// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.


#include "Core/PFEGameInstance.h"

#include "Core/SavingSystem/PFESaveGame.h"
#include "Core/SavingSystem/PFESaveGameParameters.h"
#include "Core/SavingSystem/Saveable.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FlameComponent.h"
#include "Player/PFECharacter.h"

void UPFEGameInstance::Init()
{
	Super::Init();

	// UE_LOG(LogTemp, Warning, TEXT("GameInstance::Init"));
	//
	// bHasSaveFile = CheckSaveFile();
	//
	// if (bHasSaveFile)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("GameInstance::Init HAS save game file"));
	// 	CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("GameInstance::Init DOES NOT have save game file"));
	// 	CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(USaveGame::StaticClass()));
	// }
	//
	// if (UGameplayStatics::DoesSaveGameExist(SlotNameParam, UserIndex))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("GameInstance::Init HAS save param file"));
	// 	CurrentSaveParam = Cast<UPFESaveGameParameters>(UGameplayStatics::LoadGameFromSlot(SlotNameParam, UserIndex));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("GameInstance::Init DOES NOT have save param file"));
	// 	CurrentSaveParam = Cast<UPFESaveGameParameters>(UGameplayStatics::CreateSaveGameObject(USaveGame::StaticClass()));
	// }
}

void UPFEGameInstance::LoadGameDatasSync()
{
	if (CheckSaveFile() == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::LoadGameDatasSync have a save game file, load it"));
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));	
	}
	
	if (CurrentSave == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::LoadGameDatasSync fail to load file"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("GameInstance::LoadGameDatasSync start loading"));
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveable::StaticClass(), FoundActors);

	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (APFECharacter* Player = Cast<APFECharacter>(Character))
		{
			Player->SetActorTransform(CurrentSave->PlayerData.PlayerTransform);
			Player->GetFlameComponent()->SetFlameStatus(CurrentSave->PlayerData.FlameStatus);
			// TODO : init without sounds
			if (CurrentSave->PlayerData.bHasKey) Player->StoreKey(1);
			else Player->StoreKey(0);
			Player->bIsAlive = true;

			UE_LOG(LogTemp, Warning, TEXT("save game :: player location (%d, %d)"),(int)Player->GetTransform().GetLocation().X, (int)Player->GetTransform().GetLocation().Z);
			UE_LOG(LogTemp, Warning, TEXT("world value :: player location (%d, %d)"),(int)Player->GetTransform().GetLocation().X, (int)Player->GetTransform().GetLocation().Z);
		}
	}

	for (AActor* SaveActor : FoundActors)
	{
		if (SaveActor->Implements<USaveable>())
		{
			if (FSaveActorDatas* Data = CurrentSave->SavedActors.Find(ISaveable::Execute_GetActorID(SaveActor)))
			{
				UE_LOG(LogTemp, Warning, TEXT("Actor %s loading"), *SaveActor->GetActorLabel())
				if (USceneComponent* RootComp = SaveActor->GetRootComponent())
				{
					if (RootComp->Mobility != EComponentMobility::Static)
					{
						SaveActor->SetActorTransform(Data->ActorTransform);
						UE_LOG(LogTemp, Warning, TEXT("%s data location (%d, %d)"),*Data->ActorID, (int)Data->ActorTransform.GetLocation().X, (int)Data->ActorTransform.GetLocation().Z);
						UE_LOG(LogTemp, Warning, TEXT("%s location (%d, %d)"),*SaveActor->GetActorLabel(), (int)SaveActor->GetTransform().GetLocation().X, (int)SaveActor->GetTransform().GetLocation().Z);
					}
				}
				ISaveable::Execute_OnLoad(SaveActor, Data->BinaryDatas);
			}
		}
	}

	LoadGameFinished.Broadcast();
}

void UPFEGameInstance::LoadGameDatasWithDelaySync(float DelayNextActions)
{
	FTimerHandle LoadHandle;
	GetWorld()->GetTimerManager().SetTimer(LoadHandle, this, &UPFEGameInstance::LoadGame, DelayNextActions, false);
}

void UPFEGameInstance::SaveGameDatasASync()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::SaveGameDatasASync"));

	if (!CurrentSave)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::SaveGameDatasASync DOES NOT have save game file, create one"));
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(UPFESaveGame::StaticClass()));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::SaveGameDatasASync has a save file to write datas"));
	
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (APFECharacter* Player = Cast<APFECharacter>(Character))
		{
			CurrentSave->PlayerData.PlayerTransform = Player->GetTransform();
			CurrentSave->PlayerData.FlameStatus = Player->GetFlameComponent()->GetFlameStatus();
			CurrentSave->PlayerData.bHasKey = Player->HasKey();

			UE_LOG(LogTemp, Warning, TEXT("world value :: player location (%d, %d)"),(int)Player->GetTransform().GetLocation().X, (int)Player->GetTransform().GetLocation().Z);
			UE_LOG(LogTemp, Warning, TEXT("save game :: player location (%d, %d)"),(int)Player->GetTransform().GetLocation().X, (int)Player->GetTransform().GetLocation().Z);
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

				if (USceneComponent* RootComp = SaveActor->GetRootComponent())
				{
					if (RootComp->Mobility != EComponentMobility::Static)
					{
						Data.ActorTransform = SaveActor->GetTransform();
						UE_LOG(LogTemp, Warning, TEXT("%s location (%d, %d)"),*SaveActor->GetActorLabel(), (int)SaveActor->GetTransform().GetLocation().X, (int)SaveActor->GetTransform().GetLocation().Z);
						UE_LOG(LogTemp, Warning, TEXT("%s data location (%d, %d)"),*Data.ActorID, (int)Data.ActorTransform.GetLocation().X, (int)Data.ActorTransform.GetLocation().Z);
					}
				}
				
				TArray<uint8> TempData;
				ISaveable::Execute_OnSave(SaveActor, TempData);
				Data.BinaryDatas = MoveTemp(TempData); // no copy, move memory
				
				CurrentSave->SavedActors.Add(Data.ActorID, Data);
			}
		}
	}
	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindUObject(this, &UPFEGameInstance::OnSaveAsyncGameFinished);
	UGameplayStatics::AsyncSaveGameToSlot(CurrentSave, SlotName, UserIndex, SaveDelegate);
	
}

void UPFEGameInstance::SaveGameDatasASync(FVector Location)
{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::SaveGameDatasASync"));

	if (!CurrentSave)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::SaveGameDatasASync DOES NOT have save game file, create one"));
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(UPFESaveGame::StaticClass()));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::SaveGameDatasASync has a save file to write datas"));
	
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (APFECharacter* Player = Cast<APFECharacter>(Character))
		{
			FTransform NewTransform = FTransform(Location);
			CurrentSave->PlayerData.PlayerTransform = NewTransform;
			CurrentSave->PlayerData.FlameStatus = Player->GetFlameComponent()->GetFlameStatus();
			CurrentSave->PlayerData.bHasKey = Player->HasKey();

			UE_LOG(LogTemp, Warning, TEXT("world value :: player location (%d, %d)"),(int)Player->GetTransform().GetLocation().X, (int)Player->GetTransform().GetLocation().Z);
			UE_LOG(LogTemp, Warning, TEXT("save game :: player location (%d, %d)"),(int)Player->GetTransform().GetLocation().X, (int)Player->GetTransform().GetLocation().Z);
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

				if (USceneComponent* RootComp = SaveActor->GetRootComponent())
				{
					if (RootComp->Mobility != EComponentMobility::Static)
					{
						Data.ActorTransform = SaveActor->GetTransform();
						UE_LOG(LogTemp, Warning, TEXT("%s location (%d, %d)"),*SaveActor->GetActorLabel(), (int)SaveActor->GetTransform().GetLocation().X, (int)SaveActor->GetTransform().GetLocation().Z);
						UE_LOG(LogTemp, Warning, TEXT("%s data location (%d, %d)"),*Data.ActorID, (int)Data.ActorTransform.GetLocation().X, (int)Data.ActorTransform.GetLocation().Z);
					}
				}
				
				TArray<uint8> TempData;
				ISaveable::Execute_OnSave(SaveActor, TempData);
				Data.BinaryDatas = MoveTemp(TempData); // no copy, move memory
				
				CurrentSave->SavedActors.Add(Data.ActorID, Data);
			}
		}
	}
	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindUObject(this, &UPFEGameInstance::OnSaveAsyncGameFinished);
	UGameplayStatics::AsyncSaveGameToSlot(CurrentSave, SlotName, UserIndex, SaveDelegate);
}

void UPFEGameInstance::SavePlayerLocationAsync(FVector Location)
{
	if (!CurrentSave) return;

	FTransform PlayerTransform = FTransform(Location);

	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (APFECharacter* Player = Cast<APFECharacter>(Character))
		{
			CurrentSave->PlayerData.PlayerTransform = FTransform(Location);
			CurrentSave->PlayerData.FlameStatus = EFlameStatus::NORMAL;
			CurrentSave->PlayerData.bHasKey = false;

			UE_LOG(LogTemp, Warning, TEXT("world value :: player location (%d, %d)"),(int)Player->GetTransform().GetLocation().X, (int)Player->GetTransform().GetLocation().Z);
			UE_LOG(LogTemp, Warning, TEXT("save game :: player location (%d, %d)"),(int)Player->GetTransform().GetLocation().X, (int)Player->GetTransform().GetLocation().Z);
		}
	}
	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindUObject(this, &UPFEGameInstance::OnSaveAsyncGameFinished);
	UGameplayStatics::AsyncSaveGameToSlot(CurrentSave, SlotName, UserIndex, SaveDelegate);
}

bool UPFEGameInstance::CheckSaveFile()
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

void UPFEGameInstance::ClearSaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ClearSaveGame"));
	if (CheckSaveFile() == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::ClearSaveGame - delete game slot"));
		UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
		CurrentSave = nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::ClearSaveGame - create save game slot"));
	CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(UPFESaveGame::StaticClass()));
}

void UPFEGameInstance::ResetSaveGameFile()
{
	if (CheckSaveFile() == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::ResetSaveGameFile - delete game slot"));
		UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
		CurrentSave = nullptr;
	}
}

void UPFEGameInstance::LoadPreGameDatas()
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotNameParam, UserIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::LoadPreGameDatas HAS save param file"));
		CurrentSaveParam = Cast<UPFESaveGameParameters>(UGameplayStatics::LoadGameFromSlot(SlotNameParam, UserIndex));
	}
	
	if (!CurrentSaveParam) return;
	
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::LoadPreGameDatas load datas"));
	SoundLevelMaster = CurrentSaveParam->SoundsVolume.MasterVolume;
	SoundLevelMusic = CurrentSaveParam->SoundsVolume.MusicVolume;
	SoundLevelSFX = CurrentSaveParam->SoundsVolume.SFXVolume;
	SoundLevelAmbiance = CurrentSaveParam->SoundsVolume.AmbianceVolume;
}

void UPFEGameInstance::SavePreGameDatas()
{
	if (!CurrentSaveParam)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::SavePreGameDatas DOES NOT have save param file, create one"));
		CurrentSaveParam = Cast<UPFESaveGameParameters>(UGameplayStatics::CreateSaveGameObject(UPFESaveGameParameters::StaticClass()));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::SavePreGameDatas"));
	CurrentSaveParam->SoundsVolume.MasterVolume = SoundLevelMaster;
	CurrentSaveParam->SoundsVolume.MusicVolume = SoundLevelMusic;
	CurrentSaveParam->SoundsVolume.SFXVolume = SoundLevelSFX;
	CurrentSaveParam->SoundsVolume.AmbianceVolume = SoundLevelAmbiance;

	UGameplayStatics::AsyncSaveGameToSlot(CurrentSaveParam, SlotNameParam, UserIndex);
}

void UPFEGameInstance::RegisterToSave(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::RegisterToSave %s"), *Actor->GetActorLabel());
	ActorsToSave.Add(Actor);
}

void UPFEGameInstance::UnregisterFromSave(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::UnregisterFromSave %s"), *Actor->GetActorLabel());
	ActorsToSave.Remove(Actor);
}

void UPFEGameInstance::OnSaveAsyncGameFinished(const FString& InSlotName, const int32 InUserIndex, bool bInSuccess)
{
	if (bInSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::OnSaveAsyncGameFinished launch event SaveGameFinished"));
		SaveGameFinished.Broadcast();
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Async save %s, failed"), *InSlotName);
}

void UPFEGameInstance::LoadGame()
{
	bHasDied = true;
	LoadGameDatasSync();
}

