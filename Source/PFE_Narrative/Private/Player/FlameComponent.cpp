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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
	FString::Printf(TEXT("UpdateFlameValue Flame = %f"), CurrentFlameValue));
	UpdateFlameStatus();
	
	if (CurrentFlameValue >= MaxFlameValue || CurrentFlameValue <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFlameComponent::UpdateFlameValue : death"));
		CurrentFlameValue = FMath::Clamp(CurrentFlameValue, 0.f, MaxFlameValue);
		
		PFECharacter->GetGameMode()->OnDeath.Broadcast();
		OnDeathFlameState.Broadcast(CurrentFlameStatus == EFlameStatus::HIGH);
	}
}

void UFlameComponent::SetFlameValue(float Value)
{
	CurrentFlameValue = Value;
	UpdateFlameStatus();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
	FString::Printf(TEXT("SetFlameValue Flame = %f"), CurrentFlameValue));
}

void UFlameComponent::StartEffect(bool bInIsOneShot, float Value, float Delay, float DelayNormal, bool bDecrease, AArea* InAreaRef)
{
	if ((bInIsOneShot && bDecrease && CurrentFlameStatus == EFlameStatus::SMALL) ||
	(bInIsOneShot && !bDecrease && CurrentFlameStatus == EFlameStatus::HIGH))
	{
		PFECharacter->GetGameMode()->OnDeath.Broadcast();
		OnDeathFlameState.Broadcast(CurrentFlameStatus == EFlameStatus::HIGH);
		return;
	}
	
	// stop current area effect to apply new one
	if (Areas.Num() > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FlameEffectTimer);
		GetWorld()->GetTimerManager().ClearTimer(ResetFlameTimer);
	}
	// store new effect
	float EffectValue = bDecrease ? -Value : Value;
	Areas.Add(FAreaEffect(bInIsOneShot, bDecrease, EffectValue, Delay, DelayNormal, InAreaRef));
	// apply new effect
	if (bInIsOneShot)
	{
		SetFlameValue(Value);
	}
	else
	{
		LaunchEffect(EffectValue, Delay);
	}
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

void UFlameComponent::ResetFlameOverTime(float Value)
{
	if ((Value > 0 && CurrentFlameValue >= 50.f) || (Value < 0 && CurrentFlameValue <= 50.f))
	{
		CurrentFlameValue = 50.f;
		GetWorld()->GetTimerManager().ClearTimer(ResetFlameTimer);
		return;
	}
	CurrentFlameValue += Value;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
		FString::Printf(TEXT("ResetFlameOverTime Flame = %f"), CurrentFlameValue));
	UpdateFlameStatus();
}

void UFlameComponent::UpdateFlameStatus()
{
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
}

void UFlameComponent::EndEffect(bool bInIsOneShot, AArea* InAreaRef)
{
	GetWorld()->GetTimerManager().ClearTimer(FlameEffectTimer);

	int32 IndexToRemove = Areas.IndexOfByPredicate(
		[InAreaRef](const FAreaEffect& Effect)
		{
			return Effect.AreaRef == InAreaRef;
		});

	FAreaEffect CurrentArea = Areas[IndexToRemove];

	if (IndexToRemove != INDEX_NONE)
	{
		Areas.RemoveAt(IndexToRemove);
	}

	if (Areas.Num() > 0)
	{
		FAreaEffect PreviousArea = Areas.Last();
		
		if (PreviousArea.bIsOneShot)
		{
			SetFlameValue(PreviousArea.Value);
		}
		else
		{
			LaunchEffect(PreviousArea.Value, PreviousArea.Delay);
		}
	}

	if (Areas.Num() == 0)
	{
		float IncrementValue = (50.f - CurrentFlameValue) / (CurrentArea.DelayBeforeNormalFlame / 0.1f);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green,
	FString::Printf(TEXT("EndEffect : increment value = %f, current %f, delay %f"),
		IncrementValue, CurrentFlameValue, CurrentArea.DelayBeforeNormalFlame));

		GetWorld()->GetTimerManager().SetTimer(
			ResetFlameTimer,
			[this, IncrementValue]()
			{
				ResetFlameOverTime(IncrementValue);
			},
			0.1f,
			true
		);
	}
}
