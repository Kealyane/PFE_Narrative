// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Checkpoint.h"

#include "PaperSpriteComponent.h"
#include "Core/PFEGameMode.h"
#include "Core/SoundComponent.h"
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
	bIsActive = false;
	OnActorBeginOverlap.AddDynamic(this, &ACheckpoint::CheckpointReached);
}

void ACheckpoint::CheckpointReached(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr && OtherActor->IsA(APFECharacter::StaticClass()))
	{
		if (APFEGameMode* PFEGameMode = Cast<APFEGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			if (!bIsActive)
			{
				bIsActive = true;
				UpdateCheckpoint(bIsActive);
			}
			APFECharacter* Character = Cast<APFECharacter>(OtherActor);
			EFlameStatus FlameStatus = Character->GetFlameComponent()->GetFlameStatus();
			PFEGameMode->SetCheckpoint(this, GetActorLocation(), FlameStatus);
		}
	}
}
