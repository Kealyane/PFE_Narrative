// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointManager.generated.h"

class APFECharacter;
class ACheckpoint;


UCLASS()
class PFE_NARRATIVE_API ACheckpointManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ACheckpointManager();

	UPROPERTY(EditAnywhere)
	TArray<ACheckpoint*> CheckpointList;

	UFUNCTION(BlueprintCallable)
	void Next();
	UFUNCTION(BlueprintCallable)
	void Previous();

	void NotifyCheckpointIsReached(ACheckpoint* Checkpoint);
	
protected:
	virtual void BeginPlay() override;

	int CurrentIndex = 0;
	APFECharacter* Character;
};
