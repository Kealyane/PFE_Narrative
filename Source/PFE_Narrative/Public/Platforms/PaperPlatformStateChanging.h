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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Platform Settings|Debug")
	bool bShowDebug = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Platform Settings|Debug")
	FLinearColor OpenColor = FLinearColor(1.f,0.06f,0.f,1.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Platform Settings|Debug")
	FLinearColor CloseColor = FLinearColor(0.04f,1.f,0.87f,1.f);
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void SwitchCollisionPreset();

private:
	TObjectPtr<UPrimitiveComponent> PrimitiveComponent;
	
};
