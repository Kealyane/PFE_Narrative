// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PFEGameMode.h"

APFEGameMode::APFEGameMode()
{

}

void APFEGameMode::SetCheckpoint(const FVector& CheckpointPosition)
{
	LastCheckpointLocation = CheckpointPosition;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("GameMode::Checkpoint"));
	UE_LOG(LogTemp, Warning, TEXT("GameMode::Checkpoint"));
}

