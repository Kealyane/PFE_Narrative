// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/KeyPickup.h"

#include "PaperSpriteComponent.h"
#include "Core/PFEGameInstance.h"
#include "Core/PFEGameMode.h"
#include "Core/SavingSystem/UniqueIDComponent.h"
#include "Player/PFECharacter.h"

AKeyPickup::AKeyPickup()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKeyPickup::OnSave_Implementation(TArray<uint8>& OutData)
{
	FMemoryWriter Writer(OutData);
	Writer << bHasBeenTaken;
}

void AKeyPickup::OnLoad_Implementation(const TArray<uint8>& InData)
{
	FMemoryReader Reader(InData);
	Reader << bHasBeenTaken;

	bHasBeenTaken ?	SetKeyVisibility(false) : SetKeyVisibility(true);
}

FString AKeyPickup::GetActorID_Implementation()
{
	return UniqueIDComponent->ActorID;
}

void AKeyPickup::BeginPlay()
{
	Super::BeginPlay();

	if (UPFEGameInstance* PFE_GI = Cast<UPFEGameInstance>(GetWorld()->GetGameInstance()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("id : %s"), *GetActorID_Implementation())
		PFE_GI->RegisterToSave(this);
	}
	
	bHasBeenTaken = false;
	SetKeyVisibility(true);
	OnActorBeginOverlap.AddDynamic(this, &AKeyPickup::OnOverlapBegin);
}

void AKeyPickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UPFEGameInstance* PFE_GI = Cast<UPFEGameInstance>(GetWorld()->GetGameInstance()))
	{
		PFE_GI->UnregisterFromSave(this);
	}
}

void AKeyPickup::SetKeyVisibility(bool Visible)
{
	GetRenderComponent()->SetVisibility(Visible);
	SetActorEnableCollision(Visible);
}

void AKeyPickup::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor)
	{
		if (APFECharacter* Character = Cast<APFECharacter>(OtherActor))
		{
			bHasBeenTaken = true;
			Character->StoreKey(1);
			SetKeyVisibility(false);
		}
	}
}
