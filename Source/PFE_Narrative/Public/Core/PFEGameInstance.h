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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UPFESaveGame> CurrentSave;

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void LoadGameDatasSync();
	UFUNCTION(BlueprintCallable)
	void SaveGameDatasASync();

	UFUNCTION(BlueprintCallable)
	void RegisterToSave(AActor* Actor) { ActorsToSave.Add(Actor); }
	UFUNCTION(BlueprintCallable)
	void UnregisterFromSave(AActor* Actor) { ActorsToSave.Remove(Actor); }
	
private:
	const FString SlotName = "Slot01";
	const int32 UserIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHasSaveFile;

	TArray<TWeakObjectPtr<AActor>> ActorsToSave;
};
