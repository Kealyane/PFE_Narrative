// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "PaperPlatformStateChanging.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateChangedDelegate, bool, bIsPlatformOpen);

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API APaperPlatformStateChanging : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	APaperPlatformStateChanging();

	UPROPERTY(BlueprintAssignable)
	FStateChangedDelegate StateChanged;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Platform Settings")
	float DelayWhenPlayerOn = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Platform Settings")
	float DelayBeforeSwitch = 2.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Platform Settings")
	bool bIsOpen;
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void SwitchCollisionPreset();

private:
	TObjectPtr<UPrimitiveComponent> PrimitiveComponent;
};
