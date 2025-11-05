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

void UPFEEditorUtility::WriteBool(TArray<uint8>& Bytes, bool Value)
{
	FMemoryWriter Writer(Bytes, true);
	Writer.Seek(Bytes.Num());
	Writer << Value;
}

void UPFEEditorUtility::ReadBool(const TArray<uint8>& Bytes, int32& Offset, bool& OutValue)
{
	FMemoryReader Reader(Bytes, true);
	Reader.Seek(Offset);
	Reader << OutValue;
	Offset = Reader.Tell();
}
