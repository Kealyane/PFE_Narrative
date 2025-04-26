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
	default:
		Super::PhysCustom(deltaTime, Iterations);
		break;
	}
}

void UPFECharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (MovementMode == MOVE_Walking)
	{
		DashCountAir = 0;
		if (!bCanDash)
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
}

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

// V1

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
		DrawDebugPoint(GetWorld(), PFECharacterOwner->GetActorLocation(), 5.f, FColor::Yellow, false, 5.f);

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
void UPFECharacterMovementComponent::StartDash(const FVector& InDirection)
{
	if (!bCanDash) return;

	const bool bIsInAir = !IsMovingOnGround();

	if (bIsInAir && DashCountAir >= MaxDashInAir) return;

	if (bIsInAir) DashCountAir++;
	else bCanDash = false;

	PFECharacterOwner->bIsDashing = true;
	DashDirection = InDirection.GetSafeNormal();
	SetMovementMode(MOVE_Custom, (uint8)EPFEMovementMode::PFEMOVE_DASHING);

	FTimerHandle DashTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &UPFECharacterMovementComponent::StopDash, DashDurationInSec, false);
}

void UPFECharacterMovementComponent::StopDash()
{
	PFECharacterOwner->bIsDashing = false;
	SetMovementMode(MOVE_Walking);
}

void UPFECharacterMovementComponent::ResetDash()
{
	bCanDash = true;

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

void UPFECharacterMovementComponent::DebugJump()
{
	//FVector ActorLocation = PFECharacterOwner->GetActorLocation();
	FVector Up = PFECharacterOwner->GetActorUpVector();

	FVector Start = ActorJumpLocation;
	FVector EndJump = Start + Up * JumpHeight * 2;

	// Max Speed (RED)
	DrawDebugLine(GetWorld(), Start, EndJump, FColor::Red, false, -1.f, 0, 2.f);

	DrawDebugString(GetWorld(), Start + FVector(50,0,100),
	FString::Printf(TEXT("Jump Speed: %.1f"), Velocity.Z),
	nullptr, FColor::White, 0.f, true);
}
