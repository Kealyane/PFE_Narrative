// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Checkpoint.generated.h"


/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API ACheckpoint : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	ACheckpoint();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsActive = false;

	void ResetCheckpoint();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCheckpoint(bool InIsActive);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void CheckpointReached(AActor* OverlappedActor, AActor* OtherActor);
};
