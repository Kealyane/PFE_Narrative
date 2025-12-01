// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/CheckpointManager.h"
#include "Gameplay/Checkpoint.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PFECharacter.h"

ACheckpointManager::ACheckpointManager()
{
	PrimaryActorTick.bCanEverTick = false;
	CheckpointList.Empty();
}

void ACheckpointManager::Next()
{
	FVector NewLocation;
	
	if (CurrentIndex + 1 < CheckpointList.Num())
	{
		NewLocation = CheckpointList[CurrentIndex+1]->GetActorLocation();
	}
	else
	{
		NewLocation = CheckpointList[CurrentIndex]->GetActorLocation();
	}
	Character->SetActorLocation(NewLocation);
}

void ACheckpointManager::Previous()
{
	FVector NewLocation;
	
	if (CurrentIndex - 1 >= 0)
	{
		NewLocation = CheckpointList[CurrentIndex-1]->GetActorLocation();
	}
	else
	{
		NewLocation = CheckpointList[CurrentIndex]->GetActorLocation();
	}
	Character->SetActorLocation(NewLocation);
}

void ACheckpointManager::NotifyCheckpointIsReached(ACheckpoint* Checkpoint)
{
	int32 Index = CheckpointList.IndexOfByKey(Checkpoint);

	if (Index != INDEX_NONE)
	{
		CurrentIndex = Index;
	}
}

// Called when the game starts or when spawned
void ACheckpointManager::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* GlobalCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	Character = Cast<APFECharacter>(GlobalCharacter);
	//check(Character);
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("CheckpointManager::BeginPlay Character not found"));
	}
	CurrentIndex = 0;
}
