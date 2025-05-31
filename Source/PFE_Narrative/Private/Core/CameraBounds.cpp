// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CameraBounds.h"
#include "Components/BoxComponent.h"

ACameraBounds::ACameraBounds()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	TriggerZone->SetupAttachment(RootComponent);
}

bool ACameraBounds::GetBoundingBox(EDirection InDirection, float& OutValue) const
{
	FVector Location = GetActorLocation();
	FVector BoxExtent = TriggerZone->GetScaledBoxExtent();
	switch (InDirection)
	{
	case EDirection::UP:
		if (!bUp) return false;
		OutValue =  bUpBridge ? BIG_VALUE : Location.Z + BoxExtent.Z;
		return true;
	case EDirection::DOWN:
		if (!bDown) return false;
		OutValue = bDownBridge ? -BIG_VALUE : Location.Z - BoxExtent.Z;
		return true;
	case EDirection::RIGHT:
		if (!bRight) return false;
		OutValue = bRightBridge ? BIG_VALUE : Location.X + BoxExtent.X;
		return true;
	case EDirection::LEFT:
		if (!bLeft) return false;
		OutValue = bLeftBridge ? -BIG_VALUE : Location.X - BoxExtent.X;
		return true;
	}
	return false;
}

void ACameraBounds::BeginPlay()
{
	Super::BeginPlay();
}


