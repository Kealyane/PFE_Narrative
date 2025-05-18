// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PFECharacterMovementComponent.h"
#include "Player/PFECharacter.h"

UPFECharacterMovementComponent::UPFECharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPFECharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	PFECharacterOwner = Cast<APFECharacter>(GetOwner());
	check(PFECharacterOwner);

	InitVariables();
}

void UPFECharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
#if WITH_EDITOR
	if (bDebugWalkMovement)
	{
		DebugWalkAccel();
	}
#endif
}


void UPFECharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch ((EPFEMovementMode)CustomMovementMode)
	{
	case EPFEMovementMode::PFEMOVE_DASHING:
		PhysDash(deltaTime, Iterations);
		break;
	case EPFEMovementMode::PFEMOVE_WALL_GRAB:
		PhysWallGrab(deltaTime, Iterations);
		break;
	default:
		Super::PhysCustom(deltaTime, Iterations);
		break;
	}
}

void UPFECharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

#if WITH_EDITOR
	if (bDebugStateMovement)
	{
		switch (MovementMode)
		{
			case MOVE_Walking :
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Walking"));
				break;
			case MOVE_Falling :
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Falling"));
				break;
			case MOVE_Custom:
				switch ((uint8)CustomMovementMode)
				{
					case (uint8)EPFEMovementMode::PFEMOVE_DASHING:
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Dashing"));
						break;
					case (uint8)EPFEMovementMode::PFEMOVE_WALL_GRAB:
						GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Wall Grab"));
						break;
					default:
						break;
				}
				break;
			default: break;
		}
	}
#endif
	
	
	if (PreviousCustomMode == ((uint8) EPFEMovementMode::PFEMOVE_DASHING) && MovementMode == MOVE_Walking)
	{
		DashCountAir = 0;
		if (!PFECharacterOwner->bCanDash)
		{
			GetWorld()->GetTimerManager().SetTimer(DashCooldownHandle, this, &UPFECharacterMovementComponent::ResetDash, DashCooldown, false);
		}
	}
	if (PreviousMovementMode == MOVE_Falling && MovementMode == MOVE_Walking)
	{
		PFECharacterOwner->NotifyGround();

#if WITH_EDITOR
		if (bDebugJumpMovement)
		{
			FVector LandLocation = PFECharacterOwner->GetActorLocation();
			const float Distance = FVector::Dist2D(ActorJumpLocation, LandLocation);
			FString DistanceStr = FString::Printf(TEXT("Jump Distance: %.1f cm"), Distance);

			DrawDebugString(GetWorld(), (ActorJumpLocation + LandLocation) * 0.5f + FVector(0.f, 0.f, 50.f), 
						DistanceStr,nullptr, FColor::White,	5.f, true);
			
			DrawDebugSphere(GetWorld(), LandLocation, 20.f, 12, FColor::Red, false, 5.f);
		}
#endif
	}
	if (PFECharacterOwner)
	{
		if (MovementMode == MOVE_Falling)
		{
			PFECharacterOwner->bIsOnGround = false;
		}
		if (MovementMode == MOVE_Walking)
		{
			PFECharacterOwner->bIsOnGround = true;
		}
		
	}
}

float UPFECharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsFalling())
	{
		return AccelInAir;
	}
	return Super::GetMaxAcceleration();
}

float UPFECharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (IsFalling())
	{
		return DecelInAir;
	}
	return Super::GetMaxBrakingDeceleration();
}

// void UPFECharacterMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult,
// 	bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult) const
// {
//
// 	FVector Start = CapsuleLocation;
// 	FVector End = Start - FVector(0.f, 0.f, 150.f);
// #if WITH_EDITOR
// 	if (bDebugFloorCheck)
// 	{
// 		DrawDebugSphere(GetWorld(), End, 30.f, 6, FColor::Magenta, false, 5.f);
// 		DrawDebugLine(GetWorld(), Start, End, FColor::Magenta, false, 1.f, 0, 2.f);
// 	}
// #endif
// 	FCollisionQueryParams QueryParams;
// 	QueryParams.AddIgnoredActor(PFECharacterOwner);
//
// 	FHitResult Hit;
// 	bool bHit = GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity,
// 	ECC_Visibility, FCollisionShape::MakeSphere(30.f), QueryParams);
//
// 	if (bHit)
// 	{
// 		bool bWalkable = IsWalkable(Hit);
//
// 		OutFloorResult.bBlockingHit = true;
// 		OutFloorResult.FloorDist = (Hit.ImpactPoint - CapsuleLocation).Size();
// 		OutFloorResult.LineDist = OutFloorResult.FloorDist;
// 		OutFloorResult.bWalkableFloor = bWalkable;
// 		OutFloorResult.HitResult = Hit;
// 	}
// 	else
// 	{
// 		OutFloorResult.Clear();
// 	}
// }

void UPFECharacterMovementComponent::InitVariables()
{
	// WALK
	MaxWalkSpeed = WalkMaxSpeed;
	MaxAcceleration = WalkAcceleration;
	BrakingDecelerationWalking = WalkDeceleration;
	GroundFriction = WalkGroundFriction;
}

void UPFECharacterMovementComponent::PhysDash(float DeltaTime, int32 Iterations)
{
	Velocity = DashDirection * (DashDistance/DashDurationInSec);

	FHitResult Hit;
	SafeMoveUpdatedComponent(Velocity * DeltaTime, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.IsValidBlockingHit()) StopDash();
}

void UPFECharacterMovementComponent::PhysFalling(float deltaTime, int32 Iterations)
{
	Super::PhysFalling(deltaTime, Iterations);

	if (Velocity.Z > 0 && Velocity.Z < JumpSpeedAtApexThreshold)
	{
		GravityScale = GlobalGravityScale;
	}
	else if (FMath::Abs(Velocity.Z) < JumpSpeedAtApexThreshold)
	{
		GravityScale = GlobalGravityScale * GravityAtApexMult;
	}
	else
	{
		GravityScale = GlobalGravityScale * FallGravityMult;
		Velocity.Z = FMath::Max(Velocity.Z, -MaxFallSpeed);
	}
	
#if WITH_EDITOR
	if (bDebugJumpMovement)
	{
		DrawDebugPoint(GetWorld(), PFECharacterOwner->GetActorLocation(), 5.f, FColor::Black, false, 5.f);

		if (!bRecordedApex && Velocity.Z <= 0.f)
		{
			DrawDebugSphere(GetWorld(), PFECharacterOwner->GetActorLocation(), 20.f, 12, FColor::Blue, false, 5.0f);
			ApexLocation = PFECharacterOwner->GetActorLocation();
			bRecordedApex = true;
			
			const float Height = ApexLocation.Z - ActorJumpLocation.Z;
			FString HeightStr = FString::Printf(TEXT("Jump Height: %.1f cm"), Height);
			DrawDebugString(GetWorld(), ApexLocation + FVector(20.f, 0.f, 50.f), 
			HeightStr,nullptr, FColor::White,	5.f, true);
		}
	}
#endif
}

void UPFECharacterMovementComponent::PhysWallGrab(float DeltaTime, int32 Iterations)
{
	Velocity = FVector::ZeroVector;
}

void UPFECharacterMovementComponent::StartDash(const FVector& InDirection)
{
	const bool bIsInAir = !IsWalking();

	if (bIsInAir && DashCountAir >= MaxDashInAir)
	{
		PFECharacterOwner->bCanDash = false;
		return;
	}

	if (bIsInAir) DashCountAir++;
	else PFECharacterOwner->bCanDash = false;
	
	if (PFECharacterOwner->GetHasStartWallJump())
	{
		DashDirection = PFECharacterOwner->GetWallNormal();
	}
	else if (CustomMovementMode == (uint8)EPFEMovementMode::PFEMOVE_WALL_GRAB)
	{
		DashDirection = PFECharacterOwner->GetWallNormal();
		PFECharacterOwner->bIsGrabbingWall = false;
		StopWallGrab();
		PFECharacterOwner->LockInput();
		PFECharacterOwner->FlipCharacter(DashDirection.X);
	}
	else
	{
		DashDirection = InDirection;
	}
	
	
	PFECharacterOwner->bIsDashing = true;
	PFECharacterOwner->bIsJumping = false;
	DashDirection = DashDirection.GetSafeNormal();
	SetMovementMode(MOVE_Custom, (uint8)EPFEMovementMode::PFEMOVE_DASHING);
	
	FTimerHandle DashTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UPFECharacterMovementComponent::StopDash, DashDurationInSec, false);
}

void UPFECharacterMovementComponent::StopDash()
{
	PFECharacterOwner->bIsDashing = false;
	
	if (IsMovingOnGround() || MovementMode == MOVE_Walking)
	{
		SetMovementMode(MOVE_Walking);
	}
	else
	{
		SetMovementMode(MOVE_Falling);
	}
}

void UPFECharacterMovementComponent::ResetDash()
{
	PFECharacterOwner->bCanDash = true;
	DashCountAir = 0;
}

void UPFECharacterMovementComponent::StartTimerResetDash()
{
	FTimerHandle DashTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UPFECharacterMovementComponent::ResetDash, DashCooldown, false);
}

void UPFECharacterMovementComponent::StartJump()
{
	if (PFECharacterOwner->JumpCount >= MaxJumpCount) return;
	
	PFECharacterOwner->JumpCount++;

	PFECharacterOwner->bIsJumping = true;

	Velocity.Z = (2 * JumpHeight) / JumpTimeToApex;

	ApexTimeRemaining = 0.f;
	PreviousVelocityZ = Velocity.Z;
	
	SetMovementMode(MOVE_Falling);

#if WITH_EDITOR
	if (bDebugJumpMovement)
	{
		ActorJumpLocation = PFECharacterOwner->GetActorLocation();
		bRecordedApex = false;
	}
#endif
}

void UPFECharacterMovementComponent::StartWallGrab()
{
	SetMovementMode(MOVE_Custom, (uint8)EPFEMovementMode::PFEMOVE_WALL_GRAB);
	GravityScale = 0.f;
}

void UPFECharacterMovementComponent::StopWallGrab()
{
	SetMovementMode(MOVE_Falling);
	GravityScale = GlobalGravityScale;
}

void UPFECharacterMovementComponent::StartWallJump(float InWallNormal)
{
	Velocity.X = InWallNormal * WallJumpDistance / WallJumpTime;
	Velocity.Z = 980 * GravityScale * WallJumpTime;
	
#if WITH_EDITOR
	if (bDebugWallMovement)
	{
		FVector StartLocation = GetActorLocation();
		FVector SimulatedVelocity = Velocity;

		for (int32 i = 0; i < 7; ++i)
		{
			FVector NextLocation = StartLocation + SimulatedVelocity * 0.05f;
			DrawDebugLine(GetWorld(), StartLocation, NextLocation, FColor::Yellow, false, 2.0f, 0, 4.0f);
    
			SimulatedVelocity += FVector(0.f, 0.f, -980.f) * 0.05f; 
			StartLocation = NextLocation;
		}
	}
#endif
}

void UPFECharacterMovementComponent::ResetVariables()
{
	DashCountAir = 0;
	GetWorld()->GetTimerManager().ClearTimer(DashCooldownHandle);

#if WITH_EDITOR
	bRecordedApex = false;
	ApexTimeRemaining = 0.f;
	PreviousVelocityZ = 0.f;
#endif
}

void UPFECharacterMovementComponent::DebugWalkAccel()
{
	if (PFECharacterOwner)
	{
		FVector ActorLocation = PFECharacterOwner->GetActorLocation();
		FVector Forward = PFECharacterOwner->GetActorForwardVector();
		
		float CurrentSpeed = FVector::DotProduct(Velocity, Forward);
		
		FVector Start = ActorLocation + FVector(100, 0, 100); 
		FVector EndCurrent = Start + Forward * CurrentSpeed * 0.1f;
		FVector EndTarget = Start + Forward * MaxWalkSpeed * 0.1f;

		// Max Speed (RED)
		DrawDebugLine(GetWorld(), Start, EndTarget, FColor::Red, false, -1.f, 0, 2.f);
		
		// Current Speed (GREEN)
		DrawDebugLine(GetWorld(), Start, EndCurrent, FColor::Green, false, -1.f, 0, 5.f);
		
		DrawDebugString(GetWorld(), Start + FVector(100,0,0),
			FString::Printf(TEXT("Speed: %.1f / %.1f"), CurrentSpeed, MaxWalkSpeed),
			nullptr, FColor::White, 0.f, true);
	}
}
