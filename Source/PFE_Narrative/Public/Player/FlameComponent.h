// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "PaperSpriteComponent.h"
#include "Components/ActorComponent.h"
#include "FlameComponent.generated.h"


class AArea;
class APFEGameMode;

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
};
