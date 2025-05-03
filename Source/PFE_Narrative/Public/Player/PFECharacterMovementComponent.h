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
	PFEMOVE_DASHING		UMETA(DisplayName="Dashing"),
	PFEMOVE_WALL_GRAB	UMETA(DisplayName="Wall Grab"),
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
	
	float GetWallCoyoteTime() const { return WallCoyoteTime; }
	float GetWallJumpBuffer() const { return WallInputBuffer; }
	float GetWallSphereRadius() const { return WallDetectionSphereRadius; }
	float GetWallDistance() const { return WallDetectionDistance; }
	float GetWallBlockInputDelay() const { return WallJumpBlockInputDelay; }
	float GetWallDetachDelay() const { return WallDetachDelay; }

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|InAir",
		meta = (AllowPrivateAccess = "true", ToolTip = "Multiplier applied to acceleration rate in air"))
	float AccelInAir;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|InAir",
		meta = (AllowPrivateAccess = "true", ToolTip = "Multiplier applied to deceleration rate in air"))
	float DecelInAir;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Half height of jump"))
	float JumpHeight = 420.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Max jumps"))
	int MaxJumpCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Jump",
		meta = (AllowPrivateAccess = "true", ToolTip = "Time to reach the apex"))
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
		meta = (AllowPrivateAccess = "true", ToolTip = "Distance of the dash"))
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Wall",
		meta = (AllowPrivateAccess = "true", ToolTip = "Distance for wall jump"))
	float WallJumpDistance = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Wall",
		meta = (AllowPrivateAccess = "true", ToolTip = "Wall jump duration to reach WallJumpDistance"))
	float WallJumpTime = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Wall",
		meta = (AllowPrivateAccess = "true", ToolTip = "Time where player movement is blocked after a wall jump",
		ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float WallJumpBlockInputDelay;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Wall",
		meta = (AllowPrivateAccess = "true", ToolTip = "Time where player still stick to the wall if no input",
		ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float WallDetachDelay = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Wall",
		meta = (AllowPrivateAccess = "true", ToolTip = "Time to jump after leaving the wall"))
	float WallCoyoteTime = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Wall",
		meta = (AllowPrivateAccess = "true", ToolTip = "Wall buffer"))
	float WallInputBuffer = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Wall|Detection",
		meta = (AllowPrivateAccess = "true", ToolTip = "Distance to check wall"))
	float WallDetectionDistance = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Wall|Detection",
		meta = (AllowPrivateAccess = "true", ToolTip = "Sphere radius for raytrace"))
	float WallDetectionSphereRadius = 20.f;

private:

	// Dash
	uint8 DashCountAir = 0;
	FTimerHandle DashCooldownHandle;
	
	float ApexTimeRemaining = 0.f;
	float PreviousVelocityZ = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// override
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	
	void InitVariables();	

	// custom physics
	void PhysDash(float DeltaTime, int32 Iterations);
	virtual void PhysFalling(float deltaTime, int32 Iterations) override;
	void PhysWallGrab(float DeltaTime, int32 Iterations);

	FVector ActorJumpLocation;
	
public:
	// dash
	void StartDash(const FVector& InDirection);
	void StopDash();
	void ResetDash();

	// jump
	void StartJump();

	// wall grab and jump
	void StartWallGrab();
	void StopWallGrab();
	void StartWallJump(float InWallNormal);

	// DEBUG
	UPROPERTY(EditAnywhere, Category = "Movement|Debug")
	bool bDebugWalkMovement = true;
	UFUNCTION()
	void ToggleDebugWalkMovement() { bDebugWalkMovement = !bDebugWalkMovement; }
	void DebugWalkAccel();

	UPROPERTY(EditAnywhere, Category = "Movement|Debug")
	bool bDebugJumpMovement = true;
	UFUNCTION()
	void ToggleDebugJumpMovement() { bDebugJumpMovement = !bDebugJumpMovement; }
	bool bRecordedApex;
	FVector ApexLocation;
	void DebugJump();

	UPROPERTY(EditAnywhere, Category = "Movement|Debug")
	bool bDebugWallMovement = true;
	UFUNCTION()
	void ToggleDebugWallMovement() { bDebugWallMovement = !bDebugWallMovement; }
};
