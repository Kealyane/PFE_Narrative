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
