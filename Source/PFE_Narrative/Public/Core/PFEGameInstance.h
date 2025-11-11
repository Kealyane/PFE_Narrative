// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PFEGameInstance.generated.h"

class UPFESaveGameParameters;
class UPFESaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveGameFinishedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadGameFinishedSignature);
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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPFESaveGameParameters> CurrentSaveParam;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bUseSaveFile = false;

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void LoadGameDatasSync();
	UFUNCTION(BlueprintCallable)
	void SaveGameDatasASync();
	UFUNCTION(BlueprintCallable)
	bool CheckSaveFile();
	UFUNCTION(BlueprintCallable)
	void ClearSaveGame();
	
	UPROPERTY(BlueprintAssignable)
	FOnSaveGameFinishedSignature SaveGameFinished;
	UPROPERTY(BlueprintAssignable)
	FOnLoadGameFinishedSignature LoadGameFinished;

	UFUNCTION(BlueprintCallable)
	void LoadPreGameDatas();
	UFUNCTION(BlueprintCallable)
	void SavePreGameDatas();

	UFUNCTION(BlueprintCallable)
	void RegisterToSave(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	void UnregisterFromSave(AActor* Actor);

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
	const FString SlotNameParam = "SlotParam";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHasSaveFile;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHasSaveFileParam;

	TArray<TWeakObjectPtr<AActor>> ActorsToSave;
	
	void OnSaveAsyncGameFinished(const FString& SlotName, const int32 UserIndex, bool bSuccess);
};
