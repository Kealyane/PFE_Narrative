// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PFEGameMode.h"

#include "Gameplay/Checkpoint.h"


APFEGameMode::APFEGameMode()
{

}

void APFEGameMode::SetCheckpoint(ACheckpoint* InCheckpoint, const FVector& CheckpointPosition, EFlameStatus InFlameStatus)
{
	if (CurrentCheckpoint != nullptr && CurrentCheckpoint != InCheckpoint) CurrentCheckpoint->ResetCheckpoint();
	
	CurrentCheckpoint = InCheckpoint;
	LastCheckpointLocation = CheckpointPosition;
	FlameStatusAtCheckpoint = InFlameStatus;
}

void APFEGameMode::BeginPlay()
{
	Super::BeginPlay();
	CurrentCheckpoint == nullptr;
}

