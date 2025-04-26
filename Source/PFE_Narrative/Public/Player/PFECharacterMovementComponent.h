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
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API UPFECharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPFECharacterMovementComponent();
	float GetCoyoteTime() const { return CoyoteTime; }
	float GetJumpBuffer() const { return JumpBuffer; }

protected:
	TObjectPtr<APFECharacter> PFECharacterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	EPFEMovementMode CurrentMovementMode;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Gravity",
		meta = (AllowPrivateAccess = "true", ToolTip = "Gravity applied through the game"))
	float GlobalGravityScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Fall")
	float MaxFallSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Fall",
		meta = (AllowPrivateAccess = "true", ToolTip = "Multiplier to gravity scale when falling"))
	float FallGravityMult;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Walk",
		meta = (AllowPrivateAccess = "true", ToolTip = "Target speed to reach"))
	float WalkMaxSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Walk",
		meta = (AllowPrivateAccess = "true", ToolTip = "0 if none, same as MaxWalkSpeed for instant acceleration"))
	float WalkAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Walk",
		meta = (AllowPrivateAccess = "true", ToolTip = "In corelation with ground friction"))
	float WalkDeceleration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Walk",
		meta = (AllowPrivateAccess = "true", ToolTip = "Smaller value => slide"))
	float WalkGroundFriction = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Force applied upward"))
	float JumpForce = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Height of jump"))
	float JumpHeight = 420.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Max jumps"))
	int MaxJumpCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Time between applying jump force and reaching the apex"))
	float JumpTimeToApex = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Reduce gravity at apex",
			ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float GravityAtApexMult = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Need to be near 0 (player velocity) at the apex"))
	float JumpSpeedAtApexThreshold = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Grace period for jump",
		ClampMin = "0.01", ClampMax = "0.5", UIMin = "0.01", UIMax = "0.5"))
	float CoyoteTime = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Grace period for jump automatically",
		ClampMin = "0.01", ClampMax = "0.5", UIMin = "0.01", UIMax = "0.5"))
	float JumpBuffer = 0.15f;

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
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	
	void InitVariables();	

	void PhysDash(float DeltaTime, int32 Iterations);
	virtual void PhysFalling(float deltaTime, int32 Iterations) override;

	FVector ActorJumpLocation;
	
public:
	void StartDash(const FVector& InDirection);
	void StopDash();
	void ResetDash();
	void StartJump();

	// DEBUG
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugWalkMovement = false;
	UFUNCTION()
	void ToggleDebugWalkMovement() { bDebugWalkMovement = !bDebugWalkMovement; }
	void DebugWalkAccel();

	//UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugJumpMovement = true;
	bool bRecordedApex;
	FVector ApexLocation;
	void DebugJump();
};
