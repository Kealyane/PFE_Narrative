// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PFESaveGame.generated.h"


enum class EFlameStatus : uint8;

USTRUCT(BlueprintType)
struct FPlayerDatas
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform PlayerTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFlameStatus FlameStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasKey;
};

USTRUCT(BlueprintType)
struct FSaveActorDatas
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString ActorID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform ActorTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<uint8> BinaryDatas;
};

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API UPFESaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FPlayerDatas PlayerData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FString, FSaveActorDatas> SavedActors;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 LevelToLoad;
};
