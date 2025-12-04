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
			Character->EnterReflexionArea();
			if (bIsReflexionHorizontal)
				Character->SetReflexionArea(true, bIsReflexionHorizontal, GetActorLocation(), DistanceMult);
			else
				Character->SetReflexionArea(true, bIsReflexionHorizontal, GetActorLocation(), DistanceMult);
		}
	}
}

void APlatformInvisibleBounds::ExitInvisibleArea(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor)
	{
		if (APFECharacter* Character = Cast<APFECharacter>(OtherActor))
		{
			Character->ExitReflexionArea();
			if (bIsReflexionHorizontal)
				Character->SetReflexionArea(false, bIsReflexionHorizontal, FVector::Zero(), DistanceMult);
			else
				Character->SetReflexionArea(false, bIsReflexionHorizontal, FVector::Zero(), DistanceMult);
		}
	}
}
