// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "PFECharacter.generated.h"

class UCharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
struct FInputActionValue;

USTRUCT(BlueprintType)
struct FCharacterMetrix
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Max Walk Speed"))
	float MoveSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Jump Z Velocity"))
	float JumpForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 JumpMaxCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashForce;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsAlive = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanMove = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsOnGround = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MoveValue;

	UFUNCTION(BlueprintCallable)
	void InitMovementComponent(UCharacterMovementComponent* InMovementComponent);
	UFUNCTION(BlueprintCallable)
	void SwitchMetrixUI(bool bCheckBoxValue);
	
protected:

	uint8 JumpCount = 0;
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
	FVector DirectionUp = FVector(0.f, 0.f, 1.f);
	FVector DirectionRight = FVector(1.f, 0.f, 0.f);
	FCharacterMetrix CurrentMetrix;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	void InitGame();

	void Move(const FInputActionValue& Value);
	void JumpStart(const FInputActionValue& Value);
	void JumpEnd(const FInputActionValue& Value);
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	
	void SwitchMetrix(FCharacterMetrix NewMetrix);
};




