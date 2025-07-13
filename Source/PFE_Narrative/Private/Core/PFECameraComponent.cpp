// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PFECameraComponent.h"

#include "Core/CameraBounds.h"
#include "Player/PFECharacter.h"

// Sets default values for this component's properties
UPFECameraComponent::UPFECameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPFECameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPFECameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bUseDefaultCamera) return;
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PFECharacter == nullptr) return;

	// add Bias when character face right or left
	float Bias = PFECharacter->GetIsLookingRight() ? HorizontalBias : -HorizontalBias;

	float DesiredY = GetBoundTargetY();
	CurrentY = FMath::FInterpTo(CurrentY, DesiredY, DeltaTime, 1.f); 

	FVector CharacterLocation = PFECharacter->GetActorLocation();
	FVector CameraCurrentLocation = GetOwner()->GetActorLocation();
	FVector DeltaLocation = CharacterLocation - CameraCurrentLocation;

	// desired location in Dead zone
	FVector2D HalfDeadZone = DeadZoneSize * 0.5;
	bool bOutsideDeadZoneHorizontal = FMath::Abs(DeltaLocation.X) > HalfDeadZone.X;
	bool bOutsideDeadZoneVertical = FMath::Abs(DeltaLocation.Z) > HalfDeadZone.Y;

	FVector DesiredLocation = FVector(CameraCurrentLocation.X + Bias, CurrentY, CameraCurrentLocation.Z);

	// desired location outside Dead zone
	if (bOutsideDeadZoneHorizontal)
	{
		DesiredLocation.X = CharacterLocation.X - FMath::Sign(DeltaLocation.X) * HalfDeadZone.X + Bias;
	}
	if (bOutsideDeadZoneVertical)
	{
		DesiredLocation.Z = CharacterLocation.Z - FMath::Sign(DeltaLocation.Z) * HalfDeadZone.Y;
	}
	
	bool bHasBoundDown = false;

	// desired location with camera boundaries
	if (ActiveBounds.Num() > 0)
	{
		float MinX, MaxX, MinZ, MaxZ;
		float HalfWidth, HalfHeight;
		CameraHalfSize(HalfWidth, HalfHeight);

		if (FindHighPrioBoundForDirection(EDirection::LEFT, MinX))
			MinX += HalfWidth;
		else
			MinX = DesiredLocation.X - BIG_VALUE;

		if (FindHighPrioBoundForDirection(EDirection::RIGHT, MaxX))
			MaxX -= HalfWidth;
		else
			MaxX = DesiredLocation.X + BIG_VALUE;
		
		if (FindHighPrioBoundForDirection(EDirection::DOWN, MinZ))
		{
			MinZ += HalfHeight;
			bHasBoundDown = true;
		}
		else
		{
			MinZ = DesiredLocation.Z - BIG_VALUE;
		}

		if (FindHighPrioBoundForDirection(EDirection::UP, MaxZ))
			MaxZ -= HalfHeight;
		else
			MaxZ = DesiredLocation.Z + BIG_VALUE;

		if (MaxX > MinX && MaxZ > MinZ)
		{
			DesiredLocation.X = FMath::Clamp(DesiredLocation.X, MinX, MaxX);
			DesiredLocation.Z = FMath::Clamp(DesiredLocation.Z, MinZ, MaxZ);
		}
	}

	// choose interpolation speed
	float HorizontalSpeed = bOutsideDeadZoneHorizontal && PFECharacter->MoveValue > 0.1f
						? InterpSpeedFast    
						: InterpSpeedSlow; 

	float VerticalSpeed = DeltaLocation.Z < 0 ? VerticalInterpSpeedFast : InterpSpeedSlow;

	// interpolate camera position
	FVector InterpolatedLocation;

	// Horizontal
	InterpolatedLocation.X = FMath::FInterpTo(
		CameraCurrentLocation.X,
		DesiredLocation.X,
		DeltaTime,
		HorizontalSpeed
	);

	// Vertical
	if (!bHasBoundDown && DeltaLocation.Z < 0)
	{
		// focus on chara if no bounds down
		InterpolatedLocation.Z = FMath::FInterpTo(
			CameraCurrentLocation.Z,
			CharacterLocation.Z,
			DeltaTime,
			VerticalSpeed
		);
	}
	else
	{
		InterpolatedLocation.Z = FMath::FInterpTo(
			CameraCurrentLocation.Z,
			DesiredLocation.Z,
			DeltaTime,
			VerticalSpeed
		);
	}
	
	InterpolatedLocation.Y = CurrentY;

	GetOwner()->SetActorLocation(InterpolatedLocation);

// #if WITH_EDITOR
// 	{
// 		FVector CameraLocation = GetOwner()->GetActorLocation();
// 		FVector Start = FVector(CameraLocation.X, 5.f, -10000.f);
// 		FVector End   = FVector(CameraLocation.X, 5.f, 10000.f);
// 		DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, -1.0f, 0, 5.f);
// 		
// 		FVector DeadZoneCenter = FVector(CameraLocation.X - 2*Bias, 5.f, CameraLocation.Z);
// 		FVector DeadZoneExtent = FVector(DeadZoneSize.X * 0.5f, 5.f, DeadZoneSize.Y * 0.5f);
//
// 		DrawDebugBox(GetWorld(), DeadZoneCenter, DeadZoneExtent, FColor::Cyan, false, -1.f, 0, 4.f);
// 	}
// #endif
}

void UPFECameraComponent::CameraHalfSize(float& OutHalfWidth, float& OutHalfHeight) const
{
	FVector2D ScreentSize = FVector2D::ZeroVector;
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetLocalPlayer())
	{
		PC->GetLocalPlayer()->ViewportClient->GetViewportSize(ScreentSize);
	}
	float Ratio = ScreentSize.X / ScreentSize.Y;
	float FovRad = FMath::DegreesToRadians(90);
	OutHalfHeight = 0.5f * CurrentY /FMath::Tan(0.5F * FovRad);
	OutHalfWidth = OutHalfHeight * Ratio;
}

bool UPFECameraComponent::FindHighPrioBoundForDirection(EDirection Dir, float& OutValue) const
{
	bool bFound = false;
	int32 BestPriority = -1;
	float BestValue = (Dir == EDirection::LEFT || Dir == EDirection::DOWN) ? BIG_VALUE : -BIG_VALUE;

	for (ACameraBounds* Bound : ActiveBounds)
	{
		float BoundValue;
		if (!Bound->GetBoundingBox(Dir, BoundValue)) continue;
		
		// highest priority
		if (Bound->LayerPriority > BestPriority)
		{
			BestPriority = Bound->LayerPriority;
			BestValue = BoundValue;
			bFound = true;
		}
		// same priority
		else if (Bound->LayerPriority == BestPriority)
		{
			if (Dir == EDirection::LEFT || Dir == EDirection::DOWN)
			{
				if (BoundValue < BestValue) 
				{
					BestValue = BoundValue;
					bFound = true;
				}
			}
			else
			{
				if (BoundValue > BestValue)
				{
					BestValue = BoundValue;
					bFound = true;
				}
			}
		}
	}
	if (bFound)
	{
		OutValue = BestValue;
	}
	return bFound;
}

float UPFECameraComponent::GetBoundTargetY() const
{
	int32 BestPriority = -1;
	float LocalTargetY = YLocation;

	for (ACameraBounds* Bound : ActiveBounds)
	{
		if (!Bound || !Bound->bUseDistance) continue;

		if (Bound->LayerPriority > BestPriority)
		{
			LocalTargetY = Bound->DistanceToPlayer;
			BestPriority = Bound->LayerPriority;
		}
	}

	return LocalTargetY;
}


