// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Area.generated.h"

class USoundComponent;
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API AArea : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	AArea();

	UFUNCTION(BlueprintCallable)
	void InitSoundComponent(USoundComponent* InSoundComponent)	{ SoundComponent = InSoundComponent; }
	UFUNCTION()
	USoundComponent* GetSoundComponent() { return SoundComponent; }
	
protected:

	TObjectPtr<USoundComponent> SoundComponent;
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area",
		meta = (AllowPrivateAccess = "true", ToolTip = "True if it's a one shot area, the value doesn't not change over time"))
	bool bIsOneShot = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area",
		meta = (AllowPrivateAccess = "true", ToolTip = "Zone type"))
	bool bIsDecreasingFlame = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area",meta = (AllowPrivateAccess = "true"))
	float DelayBeforeNormalFlame = 2.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|OverTime",
		meta = (AllowPrivateAccess = "true", ToolTip = "Value applied to flame, positive value"))
	float FlameImpactValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|OverTime",
		meta = (AllowPrivateAccess = "true", ToolTip = "Value applied every X seconds"))
	float DelayBetweenEffect = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|OneShot",meta = (AllowPrivateAccess = "true"))
	float PointUpValue = 75.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|OneShot",meta = (AllowPrivateAccess = "true"))
	float PointDownValue = 15.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area|OneShot",meta = (AllowPrivateAccess = "true"))
	float DisableDuration = 5.f;
	
	UFUNCTION()
	void EnterArea(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ExitArea(AActor* OverlappedActor, AActor* OtherActor);
};
