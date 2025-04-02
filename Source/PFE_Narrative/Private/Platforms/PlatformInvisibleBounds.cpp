// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms/PlatformInvisibleBounds.h"

#include "Player/PFECharacter.h"

APlatformInvisibleBounds::APlatformInvisibleBounds()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlatformInvisibleBounds::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &APlatformInvisibleBounds::EnterInvisibleArea);
	OnActorEndOverlap.AddDynamic(this, &APlatformInvisibleBounds::ExitInvisibleArea);
}

void APlatformInvisibleBounds::EnterInvisibleArea(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor)
	{
		if (APFECharacter* Character = Cast<APFECharacter>(OtherActor))
		{
			Character->SetReflexionArea(true, GetActorLocation().Z);
		}
	}
}

void APlatformInvisibleBounds::ExitInvisibleArea(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor)
	{
		if (APFECharacter* Character = Cast<APFECharacter>(OtherActor))
		{
			Character->SetReflexionArea(true, 0.f);
		}
	}
}
