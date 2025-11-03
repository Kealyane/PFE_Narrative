// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PFEEditorUtility.h"

#include "Core/PFEUtility.h"
#include "Core/PFEGameInstance.h"

bool UPFEEditorUtility::IsEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

UPFEGameInstance* UPFEEditorUtility::GetPFEGameInstance(const UObject* WorldContext)
{
	return GetTypeGameInstance<UPFEGameInstance>(WorldContext);
}
