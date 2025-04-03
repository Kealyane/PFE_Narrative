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
	
	bool bDecrease;
	float Value;
	float Delay;
	TObjectPtr<AArea> AreaRef;

	FAreaEffect() : bDecrease(false), Value(0.0f), Delay(0.0f), AreaRef(nullptr) {}
	FAreaEffect(bool InDecrease, float InValue, float InDelay, AArea* InAreaRef) :
		bDecrease(InDecrease), Value(InValue), Delay(InDelay), AreaRef(InAreaRef) {}
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
	void StartEffect(float Value, float Delay, bool bDecrease, AArea* AreaRef);

	UFUNCTION()
	void EndEffect(AArea* AreaRef);

	UPROPERTY(BlueprintAssignable)
	FSmallFlameSignature OnSmallFlame;
	UPROPERTY(BlueprintAssignable)
	FNormalFlameSignature OnNormalFlame;
	UPROPERTY(BlueprintAssignable)
	FHighFlameSignature OnHighFlame;
	UPROPERTY(BlueprintAssignable)
	FFlameStateOnDeathSignature OnDeathFlameState;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flame Properties", meta = (AllowPrivateAccess = "true"))
	float MaxFlameValue = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Flame Properties", meta = (AllowPrivateAccess = "true"))
	float CurrentFlameValue = 0.f;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateFlameValue(float Value);

	FTimerHandle FlameEffectTimer;
	
	UFUNCTION()
	void LaunchEffect(float Value, float Delay);

private:
	TArray<FAreaEffect> Areas;

	TObjectPtr<APFECharacter> PFECharacter;

	EFlameStatus CurrentFlameStatus;
	bool bIsDead = false;
};
