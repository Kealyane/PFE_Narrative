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
	OnNormalFlame.Broadcast();
	CurrentFlameStatus = EFlameStatus::NORMAL;
}

void UFlameComponent::BeginPlay()
{
	Super::BeginPlay();
	PFECharacter = Cast<APFECharacter>(GetOwner());
	check(PFECharacter);
	InitFlame();
}

void UFlameComponent::UpdateFlameValue(float Value)
{
	if (PFECharacter->bIsAlive == false) return;
	
	CurrentFlameValue += Value;
	
	if (CurrentFlameValue < 30.f && CurrentFlameStatus != EFlameStatus::SMALL)
	{
		OnSmallFlame.Broadcast();
		CurrentFlameStatus = EFlameStatus::SMALL;
	}
	if (CurrentFlameValue >= 30.f && CurrentFlameValue < 70.f && CurrentFlameStatus != EFlameStatus::NORMAL)
	{
		OnNormalFlame.Broadcast();
		CurrentFlameStatus = EFlameStatus::NORMAL;
	}
	if (CurrentFlameValue >= 70.f && CurrentFlameStatus != EFlameStatus::HIGH)
	{
		OnHighFlame.Broadcast();
		CurrentFlameStatus = EFlameStatus::HIGH;
	}
	
	if (CurrentFlameValue >= MaxFlameValue || CurrentFlameValue <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFlameComponent::UpdateFlameValue : death"));
		CurrentFlameValue = FMath::Clamp(CurrentFlameValue, 0.f, MaxFlameValue);
		
		PFECharacter->GetGameMode()->OnDeath.Broadcast();
		OnDeathFlameState.Broadcast(CurrentFlameStatus == EFlameStatus::HIGH);
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
