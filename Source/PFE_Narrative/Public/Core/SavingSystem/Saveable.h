// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Saveable.generated.h"

UINTERFACE(MinimalAPI)
class USaveable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PFE_NARRATIVE_API ISaveable
{
	GENERATED_BODY()

public:
	
	// C++ only

	// virtual void OnSave(FMemoryWriter& MemoryWriter) PURE_VIRTUAL(OnSave);
	// virtual void OnLoad(FMemoryReader& MemoryReader) PURE_VIRTUAL(OnLoad);
	// virtual FString GetActorID() PURE_VIRTUAL(GetActorID, return FString(););

	// C++ and Blueprint
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Save")
	void OnSave(TArray<uint8>& OutData);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Save")
	void OnLoad(const TArray<uint8>& InData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Save")
	FString GetActorID();
};
