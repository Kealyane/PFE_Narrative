// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PFEEnums.generated.h"


UENUM(BlueprintType)
enum class ESoundType : uint8
{
	Jump,
	DoubleJump,
	Dash,
	DeathBigFlame,
	DeathSmallFlame,
	Checkpoint,
	PlatformDestructible,
	AreaPointDown,
	AreaPointUp,
	AreaZoneDown,
	AreaZoneUp,
	SwitchState,
};

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API UPFEEnums : public UObject
{
	GENERATED_BODY()
};
