// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PFEGameInstance.generated.h"

class UPFESaveGame;
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API UPFEGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// SAVE
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPFESaveGame> CurrentSave;

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void LoadGameDatasSync();
	UFUNCTION(BlueprintCallable)
	void SaveGameDatasASync();
	UFUNCTION(BlueprintCallable)
	bool CheckSaveFile();

	UFUNCTION(BlueprintCallable)
	void RegisterToSave(AActor* Actor) { ActorsToSave.Add(Actor); }
	UFUNCTION(BlueprintCallable)
	void UnregisterFromSave(AActor* Actor) { ActorsToSave.Remove(Actor); }

	// AUDIO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float SoundLevelMusic = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float SoundLevelSFX = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float SoundLevelAmbiance = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	float SoundLevelMaster = 1.f;
	
private:
	// SAVE
	const FString SlotName = "Slot01";
	const int32 UserIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHasSaveFile;

	TArray<TWeakObjectPtr<AActor>> ActorsToSave;
};
