// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Area.generated.h"

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API AArea : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	AArea();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area",
		meta = (AllowPrivateAccess = "true", ToolTip = "Zone type"))
	bool bIsDecreasingFlame = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area",
		meta = (AllowPrivateAccess = "true", ToolTip = "value applied to flame, positive value"))
	float FlameImpactValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area",
		meta = (AllowPrivateAccess = "true", ToolTip = "value applied every X seconds"))
	float DelayBetweenEffect = 0.5f;
	
	UFUNCTION()
	void EnterArea(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ExitArea(AActor* OverlappedActor, AActor* OtherActor);
};
