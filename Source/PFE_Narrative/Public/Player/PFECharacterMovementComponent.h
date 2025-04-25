// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PFECharacterMovementComponent.generated.h"

// walk, navwalk, falling, swimming, flying, custom, max
UENUM(BlueprintType)
enum class EPFEMovementMode : uint8
{
	PFEMOVE_NONE		UMETA(Hidden),
	// PFEMOVE_JUMP		UMETA(DisplayName="Jump"),
	// PFEMOVE_DOUBLE_JUMP UMETA(DisplayName="Double Jump"),
	PFEMOVE_DASHING		UMETA(DisplayName="Dashing"),
	PFEMOVE_WALL_GRAB	UMETA(DisplayName="Wall Grab"),
	PFEMOVE_WALL_JUMP	UMETA(DisplayName="Wall Jump"),
	PFEMOVE_MAX			UMETA(Hidden),
};

class APFECharacter;
UCLASS()
class PFE_NARRATIVE_API UPFECharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPFECharacterMovementComponent();
protected:
	TObjectPtr<APFECharacter> PFECharacterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	EPFEMovementMode CurrentMovementMode;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Dash",
		meta = (AllowPrivateAccess = "true", ToolTip = "Direction of the dash"))
	FVector DashDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash",
		meta = (AllowPrivateAccess = "true", ToolTip = "Distance to dash"))
	float DashDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash",
		meta = (AllowPrivateAccess = "true", ToolTip = "Duration of the dash"))
	float DashDurationInSec = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash",
		meta = (AllowPrivateAccess = "true", ToolTip = "Max Dash in the air"))
	uint8 MaxDashInAir = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Dash",
		meta = (AllowPrivateAccess = "true", ToolTip = "Dash cooldown"))
	float DashCooldown = 0.7f;
private:

	// Dash
	bool bCanDash = true;
	uint8 DashCountAir = 0;
	FTimerHandle DashCooldownHandle;

protected:
	virtual void BeginPlay() override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	void PhysDash(float DeltaTime, int32 Iterations);
public:
	void StartDash(const FVector& InDirection);
	void StopDash();
	void ResetDash();
};
