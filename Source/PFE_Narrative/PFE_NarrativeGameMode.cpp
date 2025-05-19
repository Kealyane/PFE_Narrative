// Copyright Epic Games, Inc. All Rights Reserved.

#include "PFE_NarrativeGameMode.h"
#include "PFE_NarrativeCharacter.h"
#include "UObject/ConstructorHelpers.h"

APFE_NarrativeGameMode::APFE_NarrativeGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
}
