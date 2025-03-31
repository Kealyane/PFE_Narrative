// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Area.h"

#include "Player/FlameComponent.h"
#include "Player/PFECharacter.h"

AArea::AArea()
{
}

void AArea::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AArea::EnterArea);
	OnActorEndOverlap.AddDynamic(this, &AArea::ExitArea);
}

void AArea::EnterArea(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Display, TEXT("AArea::EnterArea"));
	if (OtherActor != nullptr && OtherActor->IsA(APFECharacter::StaticClass()))
	{
		APFECharacter* Character = Cast<APFECharacter>(OtherActor);
		Character->GetFlameComponent()->StartEffect(FlameImpactValue, DelayBetweenEffect, bIsDecreasingFlame,this);
	}
}

void AArea::ExitArea(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Display, TEXT("AArea::ExitArea"));
	if (OtherActor != nullptr && OtherActor->IsA(APFECharacter::StaticClass()))
	{
		APFECharacter* Character = Cast<APFECharacter>(OtherActor);
		Character->GetFlameComponent()->EndEffect(this);
	}
}
