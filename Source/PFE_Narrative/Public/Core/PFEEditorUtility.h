// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PFEEditorUtility.generated.h"

class UPFEGameInstance;

UENUM(BlueprintType)
enum class EIntensity : uint8
{
	Small  UMETA(DisplayName = "Small"),
	Medium UMETA(DisplayName = "Medium"),
	High   UMETA(DisplayName = "High")
};

UENUM(BlueprintType)
enum class EDoorKeyType : uint8
{
	Normal  UMETA(DisplayName = "Normal"),
	Trapped UMETA(DisplayName = "Trapped"),
};

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API UPFEEditorUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Editor")
	static bool IsEditor();

	UFUNCTION(BlueprintPure, Category = "Game")
	static UPFEGameInstance* GetPFEGameInstance(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category="Binary Save|Write")
	static void WriteBool(UPARAM(ref) TArray<uint8>& Bytes, bool Value);

	UFUNCTION(BlueprintCallable, Category="Binary Save|Read")
	static void ReadBool(const TArray<uint8>& Bytes, int32& Offset, bool& OutValue);
};
