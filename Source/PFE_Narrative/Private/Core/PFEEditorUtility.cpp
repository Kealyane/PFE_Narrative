// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/PFEEditorUtility.h"

bool UPFEEditorUtility::IsEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}
