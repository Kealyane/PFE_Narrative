// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PFECharacterMovementComponent.h"

UPFECharacterMovementComponent::UPFECharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPFECharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	PFECharacterOwner = Cast<APFECharacter>(GetOwner());
	check(PFECharacterOwner);
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
}
