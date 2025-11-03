// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.


#include "Core/PFEGameInstance.h"

#include "Core/SavingSystem/PFESaveGame.h"
#include "Core/SavingSystem/Saveable.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FlameComponent.h"
#include "Player/PFECharacter.h"

void UPFEGameInstance::Init()
{
	Super::Init();

	bHasSaveFile = CheckSaveFile();
	
	if (bHasSaveFile)
	{
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	}
	else
	{
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(USaveGame::StaticClass()));
	}
}

void UPFEGameInstance::LoadGameDatasSync()
{
	if (!CurrentSave) return;

	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (APFECharacter* Player = Cast<APFECharacter>(Character))
		{
			Player->SetActorTransform(CurrentSave->PlayerData.PlayerTransform);
			Player->GetFlameComponent()->SetFlameStatus(CurrentSave->PlayerData.FlameStatus);
			if (CurrentSave->PlayerData.bHasKey) Player->StoreKey(); // TODO : init without sounds
		}
	}

	for (TWeakObjectPtr<AActor>& WeakSaveActor : ActorsToSave)
	{
		if (AActor* SaveActor = WeakSaveActor.Get())
		{
			if (SaveActor->Implements<USaveable>())
			{
				if (FSaveActorDatas* Data = CurrentSave->SavedActors.Find(ISaveable::Execute_GetActorID(SaveActor)))
				{
					SaveActor->SetActorTransform(Data->ActorTransform);
					ISaveable::Execute_OnLoad(SaveActor, Data->BinaryDatas);
				}
			}
		}
	}
}

void UPFEGameInstance::SaveGameDatasASync()
{
	if (!CurrentSave) return;

	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (APFECharacter* Player = Cast<APFECharacter>(Character))
		{
			CurrentSave->PlayerData.PlayerTransform = Player->GetTransform();
			CurrentSave->PlayerData.FlameStatus = Player->GetFlameComponent()->GetFlameStatus();
			CurrentSave->PlayerData.bHasKey = Player->HasKey();
		}
	}

	CurrentSave->SavedActors.Empty();

	for (TWeakObjectPtr<AActor>& WeakSaveActor : ActorsToSave)
	{
		if (AActor* SaveActor = WeakSaveActor.Get())
		{
			if (SaveActor->Implements<USaveable>())
			{
				FSaveActorDatas Data;
				Data.ActorID = ISaveable::Execute_GetActorID(SaveActor);
				Data.ActorTransform = SaveActor->GetTransform();
				
				TArray<uint8> TempData;
				ISaveable::Execute_OnSave(SaveActor, TempData);
				Data.BinaryDatas = MoveTemp(TempData); // no copy, move memory
				
				CurrentSave->SavedActors.Add(Data.ActorID, Data);
			}
		}
	}

	UGameplayStatics::AsyncSaveGameToSlot(CurrentSave, SlotName, UserIndex);
}

bool UPFEGameInstance::CheckSaveFile()
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

