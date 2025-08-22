// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Area.h"

#include "Core/SoundComponent.h"
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
	//UE_LOG(LogTemp, Display, TEXT("AArea::EnterArea"));
	if (OtherActor != nullptr && OtherActor->IsA(APFECharacter::StaticClass()))
	{
		APFECharacter* Character = Cast<APFECharacter>(OtherActor);
		if (!Character->bIsAlive) return;
		if (bIsOneShot)
		{
			if (ZoneEffect == EZoneEffect::DECREASE) SoundComponent->PlaySound(ESoundType::AreaPointDown);
			else if (ZoneEffect == EZoneEffect::INCREASE) SoundComponent->PlaySound(ESoundType::AreaPointUp);
			
			//float FlameValue = bIsDecreasingFlame ? PointDownValue : PointUpValue;
			
			// Character->GetFlameComponent()->StartEffect(bIsOneShot, FlameValue,
			// 	DelayBeforeNormalFlame, DelayBeforeNormalFlame, bIsDecreasingFlame,this);
			Character->GetFlameComponent()->StartPointEffect(ZoneEffect,this);
		}
		else
		{
			Character->GetFlameComponent()->CurrentZoneEffect = ZoneEffect;
			Character->GetFlameComponent()->StartEffect(bIsOneShot, FlameImpactValue,
				DelayBetweenEffect, DelayBeforeNormalFlame, ZoneEffect == EZoneEffect::DECREASE,this);
		}
	}
}

void AArea::ExitArea(AActor* OverlappedActor, AActor* OtherActor)
{
	//UE_LOG(LogTemp, Display, TEXT("AArea::ExitArea"));
	if (OtherActor != nullptr && OtherActor->IsA(APFECharacter::StaticClass()))
	{
		APFECharacter* Character = Cast<APFECharacter>(OtherActor);
		if (!bIsOneShot)
		{
			Character->GetFlameComponent()->EndEffect(bIsOneShot, this);
			Character->GetFlameComponent()->CurrentZoneEffect = EZoneEffect::NORMAL;
		}
	}
}
