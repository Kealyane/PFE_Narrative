// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "KeyPickup.generated.h"

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API AKeyPickup : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	AKeyPickup();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void InitKey();
	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
};
