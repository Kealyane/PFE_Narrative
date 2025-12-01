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
}

void UPFEGameInstance::LoadGameDatasSync()
{
	if (CheckSaveFile() == true)
	{
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));	
	}
	
	if (CurrentSave == nullptr)
	{
		return;
	}
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::LoadGameDatasSync start loading"));
#endif
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveable::StaticClass(), FoundActors);

	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (APFECharacter* Player = Cast<APFECharacter>(Character))
		{
			Player->SetActorTransform(CurrentSave->PlayerData.PlayerTransform);
			Player->GetFlameComponent()->SetFlameStatus(CurrentSave->PlayerData.FlameStatus);

			if (CurrentSave->PlayerData.bHasKey) Player->StoreKey(1);
			else Player->StoreKey(0);
			Player->bIsAlive = true;
		}
	}

	for (AActor* SaveActor : FoundActors)
	{
		if (SaveActor->Implements<USaveable>())
		{
			if (FSaveActorDatas* Data = CurrentSave->SavedActors.Find(ISaveable::Execute_GetActorID(SaveActor)))
			{
#if WITH_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("Actor %s loading"), *SaveActor->GetActorLabel());
#endif
				if (USceneComponent* RootComp = SaveActor->GetRootComponent())
				{
					if (RootComp->Mobility != EComponentMobility::Static)
					{
						SaveActor->SetActorTransform(Data->ActorTransform);
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
	if (!CurrentSave)
	{
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(UPFESaveGame::StaticClass()));
	}
	
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

				if (USceneComponent* RootComp = SaveActor->GetRootComponent())
				{
					if (RootComp->Mobility != EComponentMobility::Static)
					{
						Data.ActorTransform = SaveActor->GetTransform();
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
	if (!CurrentSave)
	{
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(UPFESaveGame::StaticClass()));
	}
	
	if (ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (APFECharacter* Player = Cast<APFECharacter>(Character))
		{
			FTransform NewTransform = FTransform(Location);
			CurrentSave->PlayerData.PlayerTransform = NewTransform;
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

				if (USceneComponent* RootComp = SaveActor->GetRootComponent())
				{
					if (RootComp->Mobility != EComponentMobility::Static)
					{
						Data.ActorTransform = SaveActor->GetTransform();
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
	if (CheckSaveFile() == true)
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
		CurrentSave = nullptr;
	}
	CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(UPFESaveGame::StaticClass()));
}

void UPFEGameInstance::ResetSaveGameFile()
{
	if (CheckSaveFile() == true)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("GameInstance::ResetSaveGameFile - delete game slot"));
#endif
		UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
		CurrentSave = nullptr;
	}
}

bool UPFEGameInstance::CheckHasReachCheckpoint(uint8 Level)
{
	if (CurrentSave == nullptr)
	{
		return false;
	}
	if (CurrentSave->LevelCheckpoint.Contains(Level))
	{
		return CurrentSave->LevelCheckpoint[Level];
	}
	return false;
}

void UPFEGameInstance::SaveLevelCheckpoint(uint8 Level, bool ClearMap)
{
	if (!CurrentSave)
	{
		CurrentSave = Cast<UPFESaveGame>(UGameplayStatics::CreateSaveGameObject(UPFESaveGame::StaticClass()));
	}
	if (ClearMap)
	{
		CurrentSave->LevelCheckpoint.Empty();
		CurrentSave->LevelCheckpoint.Add(Level, false);
	}
	else
	{
		if (CurrentSave->LevelCheckpoint.Contains(Level))
		{
			CurrentSave->LevelCheckpoint[Level] = true;
		}
		else
		{
			CurrentSave->LevelCheckpoint.Add(Level, false);
		}
	}
	FAsyncSaveGameToSlotDelegate SaveDelegate;
	SaveDelegate.BindUObject(this, &UPFEGameInstance::OnSaveAsyncGameFinished);
	UGameplayStatics::AsyncSaveGameToSlot(CurrentSave, SlotName, UserIndex, SaveDelegate);
}

void UPFEGameInstance::LoadPreGameDatas()
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotNameParam, UserIndex))
	{
		CurrentSaveParam = Cast<UPFESaveGameParameters>(UGameplayStatics::LoadGameFromSlot(SlotNameParam, UserIndex));
	}
	
	if (!CurrentSaveParam) return;
	
	SoundLevelMaster = CurrentSaveParam->SoundsVolume.MasterVolume;
	SoundLevelMusic = CurrentSaveParam->SoundsVolume.MusicVolume;
	SoundLevelSFX = CurrentSaveParam->SoundsVolume.SFXVolume;
	SoundLevelAmbiance = CurrentSaveParam->SoundsVolume.AmbianceVolume;
}

void UPFEGameInstance::SavePreGameDatas()
{
	if (!CurrentSaveParam)
	{
		CurrentSaveParam = Cast<UPFESaveGameParameters>(UGameplayStatics::CreateSaveGameObject(UPFESaveGameParameters::StaticClass()));
	}
	CurrentSaveParam->SoundsVolume.MasterVolume = SoundLevelMaster;
	CurrentSaveParam->SoundsVolume.MusicVolume = SoundLevelMusic;
	CurrentSaveParam->SoundsVolume.SFXVolume = SoundLevelSFX;
	CurrentSaveParam->SoundsVolume.AmbianceVolume = SoundLevelAmbiance;

	UGameplayStatics::AsyncSaveGameToSlot(CurrentSaveParam, SlotNameParam, UserIndex);
}

void UPFEGameInstance::RegisterToSave(AActor* Actor)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::RegisterToSave %s"), *Actor->GetActorLabel());
#endif
	ActorsToSave.Add(Actor);
}

void UPFEGameInstance::UnregisterFromSave(AActor* Actor)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("GameInstance::UnregisterFromSave %s"), *Actor->GetActorLabel());
#endif
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

