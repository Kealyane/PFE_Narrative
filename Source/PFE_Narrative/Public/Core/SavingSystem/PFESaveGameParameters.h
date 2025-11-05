// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PFESaveGameParameters.generated.h"


USTRUCT(BlueprintType)
struct FSaveSoundsVolume
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MusicVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SFXVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float AmbianceVolume;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MasterVolume;
};

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API UPFESaveGameParameters : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FSaveSoundsVolume SoundsVolume;
};
