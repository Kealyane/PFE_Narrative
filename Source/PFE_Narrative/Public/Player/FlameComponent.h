// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "FlameComponent.generated.h"


class APFEGameMode;
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API UFlameComponent : public UPaperSpriteComponent
{
	GENERATED_BODY()
	
public: 
	UFlameComponent();

	void InitFlame();

	UFUNCTION(BlueprintCallable)
	float GetFlameValue()  const { return CurrentFlameValue; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Flame Properties")
	float MaxFlameValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Flame Properties")
	float CurrentFlameValue = 0.f;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateFlameValue(float Value);
};
