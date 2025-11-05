// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Door.h"

#include "Components/CapsuleComponent.h"
#include "Core/PFEGameInstance.h"
#include "Core/SavingSystem/UniqueIDComponent.h"
#include "Player/PFECharacter.h"

ADoor::ADoor()
{
	bIsOpen = false;
}

// SAVE - LOAD
void ADoor::OnSave_Implementation(TArray<uint8>& OutData)
{
	FMemoryWriter Writer(OutData);
	Writer << bIsOpen;
}

void ADoor::OnLoad_Implementation(const TArray<uint8>& InData)
{
	FMemoryReader Reader(InData);
	Reader << bIsOpen;
	
	if (bIsOpen) OpenDoorDelegate.Broadcast();
}

FString ADoor::GetActorID_Implementation() const
{
	return UniqueIDComponent->ActorID;
}
// -----------------------

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	if (UPFEGameInstance* PFE_GI = Cast<UPFEGameInstance>(GetWorld()->GetGameInstance()))
	{
		PFE_GI->RegisterToSave(this);
	}
		
	InitDoor(bIsOpen);
	OnActorBeginOverlap.AddDynamic(this, &ADoor::OnOverlapBegin);
}

void ADoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UPFEGameInstance* PFE_GI = Cast<UPFEGameInstance>(GetWorld()->GetGameInstance()))
	{
		PFE_GI->UnregisterFromSave(this);
	}
}

void ADoor::InitDoor(bool bInIsOpen)
{
	bIsOpen = bInIsOpen;
}

void ADoor::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && !bIsOpen)
	{
		if (APFECharacter* Character = Cast<APFECharacter>(OtherActor))
		{
			if (Character->HasKey())
			{
				bIsOpen = true;
				Character->UseKey();
				OpenDoorDelegate.Broadcast();
			}
		}
	}
}
