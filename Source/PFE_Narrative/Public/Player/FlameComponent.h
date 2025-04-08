// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "PaperSpriteComponent.h"
#include "Components/ActorComponent.h"
#include "FlameComponent.generated.h"


class APFECharacter;
class AArea;
class APFEGameMode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSmallFlameSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNormalFlameSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHighFlameSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlameStateOnDeathSignature, bool, bIsHighFlame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeFlameValueSignature, bool, bIsChangeFlameValueActive);

UENUM()
enum class EFlameStatus : uint8
{
	SMALL,
	NORMAL,
	HIGH,
};

USTRUCT()
struct FAreaEffect
{
	GENERATED_BODY()

	bool bIsOneShot;
	bool bDecrease;
	float Value;
	float Delay;
	float DelayBeforeNormalFlame;
	TObjectPtr<AArea> AreaRef;

	FAreaEffect() : bIsOneShot(false), bDecrease(false), Value(0.0f),
		Delay(0.0f), DelayBeforeNormalFlame(0.0), AreaRef(nullptr) {}
	
	FAreaEffect(bool InOneShot, bool InDecrease, float InValue, float InDelay, float InDelayNormal, AArea* InAreaRef) :
		bIsOneShot(InOneShot), bDecrease(InDecrease), Value(InValue),
		Delay(InDelay), DelayBeforeNormalFlame(InDelayNormal), AreaRef(InAreaRef) {}
};
/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PFE_NARRATIVE_API UFlameComponent : public UActorComponent
{
	GENERATED_BODY()
	
public: 
	UFlameComponent();

	void InitFlame();

	UFUNCTION(BlueprintCallable)
	float GetFlameValue()  const { return CurrentFlameValue; }
	
	UFUNCTION()
	void StartEffect(bool bInIsOneShot, float Value, float Delay, float DelayNormal, bool bDecrease, AArea* AreaRef);

	UFUNCTION()
	void EndEffect(bool bInIsOneShot, AArea* AreaRef);

	UPROPERTY(BlueprintAssignable)
	FSmallFlameSignature OnSmallFlame;
	UPROPERTY(BlueprintAssignable)
	FNormalFlameSignature OnNormalFlame;
	UPROPERTY(BlueprintAssignable)
	FHighFlameSignature OnHighFlame;
	UPROPERTY(BlueprintAssignable)
	FFlameStateOnDeathSignature OnDeathFlameState;
	UPROPERTY(BlueprintAssignable)
	FChangeFlameValueSignature OnChangeFlameValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SmallFlameThreshold = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BigFlameThreshold = 70.f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flame Properties", meta = (AllowPrivateAccess = "true"))
	float MaxFlameValue = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Flame Properties", meta = (AllowPrivateAccess = "true"))
	float CurrentFlameValue = 0.f;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateFlameValue(float Value);

	void SetFlameValue(float Value);

	FTimerHandle FlameEffectTimer;
	FTimerHandle ResetFlameTimer;
	
	UFUNCTION()
	void LaunchEffect(float Value, float Delay);
	UFUNCTION()
	void ResetFlameOverTime(float Value);

	void UpdateFlameStatus();

private:
	TArray<FAreaEffect> Areas;

	TObjectPtr<APFECharacter> PFECharacter;

	EFlameStatus CurrentFlameStatus;
	bool bIsDead = false;

	void CheckDeath();
	void UpdateProgressBars();
};
