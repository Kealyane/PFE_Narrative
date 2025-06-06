// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PFECharacter.generated.h"

class UFlameComponent;
class APFEGameMode;
class UTimelineComponent;
class UCharacterMovementComponent;
class UPFECharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateKeyNumberDelegate, int, NbKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateSmallFlameDelegate, float, Percent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateHighFlameDelegate, float, Percent);

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API APFECharacter : public APaperZDCharacter
{
	GENERATED_BODY()

public:
	APFECharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayBeforeRespawn = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DashAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAlive = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanMove = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsOnGround = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanDash;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDashing = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MoveValue = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsGrabbingWall = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDoingWallJump = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsJumping = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 JumpCount = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bBlockHorizontalInput = false;
	
	UPROPERTY(BlueprintAssignable)
	FJumpDelegate JumpDelegate;
	UPROPERTY(BlueprintAssignable)
	FUpdateSmallFlameDelegate UpdateSmallFlameDelegate;
	UPROPERTY(BlueprintAssignable)
	FUpdateHighFlameDelegate UpdateHighFlameDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FUpdateKeyNumberDelegate UpdateKeyNumberDelegate;

	UFUNCTION(BlueprintCallable)
	void InitMovementComponent(UCharacterMovementComponent* InMovementComponent);
	UFUNCTION(BlueprintCallable)
	void InitCapsuleComponent(UCapsuleComponent* InCapsuleComponent);
	UFUNCTION(BlueprintCallable)
	void InitReflexionPlane(UStaticMeshComponent* InReflexionPlane);
	UFUNCTION(BlueprintCallable)
	void InitFlameComponent(UFlameComponent* InFlameComponent);
	UFUNCTION(BlueprintCallable)
	void InitSoundComponent(USoundComponent* InSoundComponent) { SoundComponent = InSoundComponent; }
	UFUNCTION(BlueprintCallable)
	void NotifyGround();

	UFUNCTION()
	APFEGameMode* GetGameMode() const { return PFEGameMode; }
	UFUNCTION(BlueprintCallable)
	UFlameComponent* GetFlameComponent() const { return FlameComponent; }
	UFUNCTION()
	USoundComponent* GetSoundComponent() const { return SoundComponent; }

	UFUNCTION()
	void StoreKey();
	UFUNCTION()
	void UseKey();
	UFUNCTION()
	bool HasKey() const { return NumberOfKeyPickedUp > 0; }

	UFUNCTION()
	void SetReflexionArea(bool bIsInside, float ZPos);

	FVector GetWallNormal() const { return WallNormal; }
	bool GetHasStartWallJump() const { return bHasStartWallJump; }
	void LockInput();
	void FlipCharacter(float Direction);
	
protected:
	
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	TObjectPtr<UFlameComponent> FlameComponent;
	TObjectPtr<class USoundComponent> SoundComponent;
	
	FVector DirectionUp = FVector(0.f, 0.f, 1.f);
	FVector DirectionRight = FVector(1.f, 0.f, 0.f);

	float RawMoveInput = 0.f;

	// Wall Grab
	float DotThreshold = 0.1f;
	FVector WallNormal;
	bool bIsNearWall = false;

	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// For Wall Grab
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void InitGame();
	void InitGameMode();
	
	void Move(const FInputActionValue& Value);
	void MoveEnd(const FInputActionValue& Value);
	void JumpStart(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);

	UFUNCTION()
	void WallGrabStart();
	UFUNCTION()
	void WallGrabEnd();
	UFUNCTION()
	void WallJump();
	UFUNCTION()
	void WallJumpReset();
	UFUNCTION()
	bool CheckWall();

	UFUNCTION()
	void LaunchRespawn();
	void Respawn();

	UFUNCTION()
	void PrintOnScreen(const FString& InText);

private:
	TObjectPtr<APFEGameMode> PFEGameMode;
	TObjectPtr<UPFECharacterMovementComponent> PFEMovementComponent;
	int NumberOfKeyPickedUp = 0;
	bool bIsInReflexionArea;
	float ReflexionAreaGround;
	TObjectPtr<UStaticMeshComponent> ReflexionPlane;
	FVector ReflexionPlaneLocation;
	FRotator LeftOrientation = FRotator(0.0f, 180.0f, 0.0f);
	FRotator RightOrientation = FRotator(0.0f, 0.0f, 0.0f);
	
	float LastTimeOnGround = 0.f;
	float LastJumpInputTime = 0.f;
	float LastWallContactTime = 0.f;
	float LastWallJumpInputTime = 0.f;

	bool bHasStartWallJump = false;

	// DEBUG
public:
	UFUNCTION(Exec)
	void ToggleDebugWalkMovement();
	UFUNCTION(Exec)
	void ToggleDebugJumpMovement();
	UFUNCTION(Exec)
	void ToggleDebugWallMovement();
	UFUNCTION(Exec)
	void ToggleDebugFloorCheck();
	UFUNCTION(Exec)
	void ToggleDebugStateMovement();
};







