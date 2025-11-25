// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Checkpoint.h"

#include "PaperSpriteComponent.h"
#include "Core/PFEGameInstance.h"
#include "Core/PFEGameMode.h"
#include "Core/SoundComponent.h"
#include "Gameplay/CheckpointManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PFECharacter.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACheckpoint::ResetCheckpoint()
{
	bIsActive = false;
	UpdateCheckpoint(bIsActive);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	CheckpointManager = Cast<ACheckpointManager>(
	UGameplayStatics::GetActorOfClass(GetWorld(), ACheckpointManager::StaticClass()));

	bIsActive = false;
	CheckpointLocation = GetActorLocation();
	
	OnActorBeginOverlap.AddDynamic(this, &ACheckpoint::CheckpointReached);
}

void ACheckpoint::CheckpointReached(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("ACheckpoint::CheckpointReached"));
	if (UPFEGameInstance* GI = Cast<UPFEGameInstance>(GetGameInstance()))
	{
		if (OtherActor != nullptr && OtherActor->IsA(APFECharacter::StaticClass()))
		{
			if (APFEGameMode* PFEGameMode = Cast<APFEGameMode>(UGameplayStatics::GetGameMode(this)))
			{
				if (!bIsActive)
				{
					UE_LOG(LogTemp, Warning, TEXT("ACheckpoint::CheckpointReached active checkpoint"));
					bIsActive = true;
					if (GI->bUseSaveFile == false)
					{
						UE_LOG(LogTemp, Warning, TEXT("ACheckpoint::CheckpointReached launch save"));
						GI->SaveGameDatasASync(OtherActor->GetActorLocation());
						UpdateCheckpoint(bIsActive);
					}
				}
				
				if (CheckpointManager) CheckpointManager->NotifyCheckpointIsReached(this);
				
				APFECharacter* Character = Cast<APFECharacter>(OtherActor);
				EFlameStatus FlameStatus = Character->GetFlameComponent()->GetFlameStatus();
				PFEGameMode->SetCheckpoint(this, GetActorLocation(), FlameStatus);
			}
		}
	}
}
