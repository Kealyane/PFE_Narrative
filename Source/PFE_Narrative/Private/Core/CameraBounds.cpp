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
	if (const float* FoundValue = CachedBounds.Find(InDirection))
	{
		OutValue = *FoundValue;
		return true;
	}
	return false;
}

void ACameraBounds::BeginPlay()
{
	Super::BeginPlay();
}

void ACameraBounds::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	CachedBounds.Empty();

	FVector Location = GetActorLocation();
	FVector Extent = TriggerZone->GetScaledBoxExtent();

	if (bLeft)
	{
		CachedBounds.Add(EDirection::LEFT, bLeftBridge ? -BIG_VALUE : Location.X - Extent.X);
	}
	if (bRight)
	{
		CachedBounds.Add(EDirection::RIGHT, bRightBridge ? BIG_VALUE : Location.X + Extent.X);
	}
	if (bDown)
	{
		CachedBounds.Add(EDirection::DOWN, bDownBridge ? -BIG_VALUE : Location.Z - Extent.Z);
	}
	if (bUp)
	{
		CachedBounds.Add(EDirection::UP, bUpBridge ? BIG_VALUE : Location.Z + Extent.Z);
	}
}


