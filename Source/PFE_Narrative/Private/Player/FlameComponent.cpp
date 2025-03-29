// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FlameComponent.h"

#include "Core/PFEGameMode.h"
#include "Player/PFECharacter.h"

UFlameComponent::UFlameComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxFlameValue = 100.f;
}

void UFlameComponent::InitFlame()
{
	CurrentFlameValue = MaxFlameValue / 2.f;
}

void UFlameComponent::BeginPlay()
{
	Super::BeginPlay();

	InitFlame();
}

void UFlameComponent::UpdateFlameValue(float Value)
{
	CurrentFlameValue += Value;
	
	if (CurrentFlameValue >= MaxFlameValue || CurrentFlameValue <= 0.f)
	{
		CurrentFlameValue = FMath::Clamp(CurrentFlameValue, 0.f, MaxFlameValue);
		
		if (APFECharacter* PFECharacter = Cast<APFECharacter>(GetOwner()))
		{
			PFECharacter->GetGameMode()->OnDeath.Broadcast();
		}
	}
}
