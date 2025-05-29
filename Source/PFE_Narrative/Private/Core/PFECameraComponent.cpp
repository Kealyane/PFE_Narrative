// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PFECameraComponent.h"

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
	//GetOwner()->SetActorRotation(FRotator(0,0,-90));
}

void UPFECameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PFECharacter == nullptr) return;



	float Bias = PFECharacter->GetIsLookingRight() ? HorizontalBias : -HorizontalBias;

	FVector CharacterLocation = PFECharacter->GetActorLocation();
	FVector CameraCurrentLocation = GetOwner()->GetActorLocation();
	FVector DeltaLocation = CharacterLocation - CameraCurrentLocation;
	
	FVector2D HalfDeadZone = DeadZoneSize * 0.5;
	bool bOutsideDeadZoneHorizontal = FMath::Abs(DeltaLocation.X) > HalfDeadZone.X;
	bool bOutsideDeadZoneVertical = FMath::Abs(DeltaLocation.Z) > HalfDeadZone.Y;

	FVector DesiredLocation = FVector(CameraCurrentLocation.X + Bias, YLocation, CameraCurrentLocation.Z);
	
	if (bOutsideDeadZoneHorizontal)
	{
		DesiredLocation.X = CharacterLocation.X - FMath::Sign(DeltaLocation.X) * HalfDeadZone.X + Bias;
	}
	if (bOutsideDeadZoneVertical)
	{
		DesiredLocation.Z = CharacterLocation.Z - FMath::Sign(DeltaLocation.Z) * HalfDeadZone.Y;
	}

	float HorizontalSpeed = bOutsideDeadZoneHorizontal && PFECharacter->MoveValue > 0.1f
						? InterpSpeedFast    
						: InterpSpeedSlow; 

	float VerticalSpeed = InterpSpeedFast;

	FVector InterpolatedLocation;

	// Horizontal
	InterpolatedLocation.X = FMath::FInterpTo(
		CameraCurrentLocation.X,
		DesiredLocation.X,
		DeltaTime,
		HorizontalSpeed
	);

	// Vertical
	InterpolatedLocation.Z = FMath::FInterpTo(
		CameraCurrentLocation.Z,
		DesiredLocation.Z,
		DeltaTime,
		VerticalSpeed
	);
	InterpolatedLocation.Y = YLocation;

	GetOwner()->SetActorLocation(InterpolatedLocation);
	
	//FVector NewLocation = FMath::VInterpTo(CameraCurrentLocation, DesiredLocation, DeltaTime, InterpSpeed);
	//GetOwner()->SetActorLocation(NewLocation);

#if WITH_EDITOR
	{
		FVector CameraLocation = GetOwner()->GetActorLocation();
		FVector Start = FVector(CameraLocation.X, 5.f, -10000.f);
		FVector End   = FVector(CameraLocation.X, 5.f, 10000.f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, -1.0f, 0, 5.f);
		
		FVector DeadZoneCenter = FVector(CameraLocation.X - 2*Bias, 5.f, CameraLocation.Z);
		FVector DeadZoneExtent = FVector(DeadZoneSize.X * 0.5f, 5.f, DeadZoneSize.Y * 0.5f);

		DrawDebugBox(GetWorld(), DeadZoneCenter, DeadZoneExtent, FColor::Cyan, false, -1.f, 0, 4.f);
	}
#endif
}


