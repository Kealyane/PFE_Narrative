// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PFEEditorUtility.generated.h"

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
};
