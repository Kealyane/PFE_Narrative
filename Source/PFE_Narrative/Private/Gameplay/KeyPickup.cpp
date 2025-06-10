// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/KeyPickup.h"

#include "PaperSpriteComponent.h"
#include "Core/PFEGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PFECharacter.h"

AKeyPickup::AKeyPickup()
{
}

void AKeyPickup::BeginPlay()
{
	Super::BeginPlay();
	// if (APFEGameMode* PFEGameMode = Cast<APFEGameMode>(UGameplayStatics::GetGameMode(this)))
	// {
	// 	PFEGameMode->OnDeath.AddDynamic(this, &AKeyPickup::InitKey);
	// }
	InitKey();
	
	OnActorBeginOverlap.AddDynamic(this, &AKeyPickup::OnOverlapBegin);
}

void AKeyPickup::InitKey()
{
	GetRenderComponent()->SetVisibility(true);
	SetActorEnableCollision(true);
}

void AKeyPickup::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor)
	{
		if (APFECharacter* Character = Cast<APFECharacter>(OtherActor))
		{
			Character->StoreKey();
			SetActorEnableCollision(false);
			GetRenderComponent()->SetVisibility(false);
			// FTimerHandle GrabHandle;
			// GetWorldTimerManager().SetTimer(
			// 	GrabHandle,
			// 	[this]() { GetRenderComponent()->SetVisibility(false); },
			// 	0.5f,
			// 	false);
		}
	}
}
