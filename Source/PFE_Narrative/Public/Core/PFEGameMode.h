// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PFEGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDeathSignature);

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API APFEGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APFEGameMode();
	
	UPROPERTY(BlueprintAssignable)
	FCharacterDeathSignature OnDeath;

	UFUNCTION()
	void SetCheckpoint(const FVector& CheckpointPosition);
	UFUNCTION()
	FVector GetCheckpointPosition() const { return LastCheckpointLocation; }

	UFUNCTION(BlueprintCallable)
	void LaunchDeathEvent() { OnDeath.Broadcast(); }
	
protected:
	FVector LastCheckpointLocation;
};
