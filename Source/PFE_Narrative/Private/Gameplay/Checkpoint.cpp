// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Checkpoint.h"

#include "PaperSpriteComponent.h"
#include "Core/PFEGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PFECharacter.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &ACheckpoint::CheckpointReached);
	GetRenderComponent()->SetVisibility(false);

}

void ACheckpoint::CheckpointReached(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != nullptr && OtherActor->IsA(APFECharacter::StaticClass()))
	{
		if (APFEGameMode* PFEGameMode = Cast<APFEGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			PFEGameMode->SetCheckpoint(GetActorLocation());
			UE_LOG(LogTemp, Display, TEXT("ACheckpoint::Checkpoint Reached"));
		}
	}
}
