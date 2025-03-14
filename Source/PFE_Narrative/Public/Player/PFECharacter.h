// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PFECharacter.generated.h"

class UTimelineComponent;
class UCharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartDashDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGrabWallDelegate, bool, bIsTouching);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpDelegate);

USTRUCT(BlueprintType)
struct FCharacterMetrix
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Max Walk Speed"))
	float MoveSpeed = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Jump Z Velocity"))
	float JumpForce = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 JumpMaxCount = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashDurationInSec = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MaxDashInAir = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashCooldown = 0.7f;
};
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metrix")
	FCharacterMetrix SmallFlamesMetrix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metrix")
	FCharacterMetrix HighFlamesMetrix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metrix")
	float GravityValue = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Metrix")
	float WallJumpForce = 600.f;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanMove = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsOnGround = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDashing = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MoveValue = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsGrabbingWall = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsJumping = false;

	UPROPERTY(BlueprintAssignable)
	FStartDashDelegate StartDashDelegate;
	UPROPERTY(BlueprintAssignable)
	FGrabWallDelegate GrabWallDelegate;
	UPROPERTY(BlueprintAssignable)
	FJumpDelegate JumpDelegate;

	UFUNCTION(BlueprintCallable)
	void InitMovementComponent(UCharacterMovementComponent* InMovementComponent);
	UFUNCTION(BlueprintCallable)
	void InitCapsuleComponent(UCapsuleComponent* InCapsuleComponent);
	UFUNCTION(BlueprintCallable)
	void SwitchMetrixUI(bool bCheckBoxValue);
	
protected:

	TObjectPtr<UCharacterMovementComponent> MovementComponent;
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	FVector DirectionUp = FVector(0.f, 0.f, 1.f);
	FVector DirectionRight = FVector(1.f, 0.f, 0.f);
	FCharacterMetrix CurrentMetrix;

	float PreviousGravity;
	// Jump
	uint8 JumpCount = 0;

	// Dash
	float DashSpeed;
	bool bCanDash;
	uint8 DashCountAir = 0;

	// Wall Grab
	float DotThreashold = 0.1f;
	float MoveThreashold = 0.01f;
	FVector WallNormal;
	bool bIsNearWall = false;
	bool bisDoingWallJump = false;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();

	// For Wall Grab
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void InitGame();
	
	void Move(const FInputActionValue& Value);
	void MoveEnd(const FInputActionValue& Value);
	void JumpStart(const FInputActionValue& Value);
	void JumpEnd(const FInputActionValue& Value);
	void Dash(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void EndDash();
	UFUNCTION()
	void ResetDash();
	UFUNCTION(BlueprintCallable)
	float GetDashDuration();
	UFUNCTION(BlueprintCallable)
	FVector GetDashVelocity();

	UFUNCTION()
	void WallGrabStart();
	UFUNCTION()
	void WallGrabEnd();
	UFUNCTION()
	void WallJump();
	UFUNCTION()
	void WallJumpReset();
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	
	void SwitchMetrix(const FCharacterMetrix& NewMetrix);
	
	UFUNCTION(BlueprintCallable)
	void EnableGravity();
	UFUNCTION(BlueprintCallable)
	void DisableGravity();

	UFUNCTION()
	void PrintOnScreen(const FString& InText);
};







