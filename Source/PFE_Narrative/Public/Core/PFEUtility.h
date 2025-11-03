// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"

template <typename T>
T* GetTypeGameInstance(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext);
	return Cast<T>(GameInstance);
}