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

void UFlameComponent::StartEffect(float Value, float Delay, bool bDecrease, AArea* InAreaRef)
{
	// stop current area effect to apply new one
	if (Areas.Num() > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FlameEffectTimer);
	}
	// store new effect
	float EffectValue = bDecrease ? -Value : Value;
	Areas.Add(FAreaEffect(bDecrease, EffectValue, Delay, InAreaRef));
	// apply new effect
	LaunchEffect(EffectValue, Delay);
}

void UFlameComponent::LaunchEffect(float Value, float Delay)
{
	GetWorld()->GetTimerManager().SetTimer(
	FlameEffectTimer, 
	[this, Value]()
	{
		UpdateFlameValue(Value);
	},
	Delay, 
	true
	);
}

void UFlameComponent::EndEffect(AArea* InAreaRef)
{
	GetWorld()->GetTimerManager().ClearTimer(FlameEffectTimer);

	int32 IndexToRemove = Areas.IndexOfByPredicate(
		[InAreaRef](const FAreaEffect& Effect)
		{
			return Effect.AreaRef == InAreaRef;
		});

	if (IndexToRemove != INDEX_NONE)
	{
		Areas.RemoveAt(IndexToRemove);
	}

	if (Areas.Num() > 0)
	{
		FAreaEffect PreviousArea = Areas.Last();
		LaunchEffect(PreviousArea.Value, PreviousArea.Delay);
	}

	if (Areas.Num() == 0)
	{
		InitFlame();
	}
}
