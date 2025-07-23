// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Player/FlameComponent.h"
#include "PFEGameMode.generated.h"

class ACheckpoint;
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
	void SetCheckpoint(ACheckpoint* InCheckpoint, const FVector& CheckpointPosition, EFlameStatus InFlameStatus);
	UFUNCTION()
	FVector GetCheckpointPosition() const { return LastCheckpointLocation; }
	UFUNCTION()
	EFlameStatus GetCheckpointFlameStatus() const { return FlameStatusAtCheckpoint; }

	UFUNCTION(BlueprintCallable)
	void LaunchDeathEvent() { OnDeath.Broadcast(); }
	
protected:
	FVector LastCheckpointLocation;
	TObjectPtr<ACheckpoint> CurrentCheckpoint;
	EFlameStatus FlameStatusAtCheckpoint;

	virtual void BeginPlay() override;
};
