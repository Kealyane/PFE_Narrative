// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FlameComponent.h"

#include "Core/PFEGameMode.h"
#include "Core/SoundComponent.h"
#include "Gameplay/Area.h"
#include "Player/PFECharacter.h"

UFlameComponent::UFlameComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxFlameValue = 100.f;
	CurrentFlameStatus = EFlameStatus::NORMAL;
}

void UFlameComponent::InitFlame()
{
	CurrentFlameValue = MaxFlameValue / 2.f;
	OnNormalFlame.Broadcast();
	CurrentFlameStatus = EFlameStatus::NORMAL;
	OnChangeFlameValue.Broadcast(false);
	PFECharacter->UpdateSmallFlameDelegate.Broadcast(0.f);
	PFECharacter->UpdateHighFlameDelegate.Broadcast(0.f);
}

void UFlameComponent::BeginPlay()
{
	Super::BeginPlay();
	PFECharacter = Cast<APFECharacter>(GetOwner());
	check(PFECharacter);
	InitFlame();
	bIsPlayingSound = false;
}

void UFlameComponent::StartEffect(bool bInIsOneShot, float Value, float Delay, float DelayNormal, bool bDecrease, AArea* InAreaRef)
{
	CheckDeath();
	
	if ((bInIsOneShot && bDecrease && CurrentFlameStatus == EFlameStatus::SMALL) ||
	(bInIsOneShot && !bDecrease && CurrentFlameStatus == EFlameStatus::HIGH))
	{
		PFECharacter->GetGameMode()->OnDeath.Broadcast();
		OnDeathFlameState.Broadcast(CurrentFlameStatus == EFlameStatus::HIGH);
		return;
	}

	if (!bInIsOneShot)
	{
		if (bIsPlayingSound)
		{
			if (bIsDownSound && !bDecrease)
			{
				InAreaRef->GetSoundComponent()->PlaySound(ESoundType::AreaZoneUp);
				AreaSoundPlaying = InAreaRef;
				bIsDownSound = false;
			}
			else if (!bIsDownSound && bDecrease)
			{
				InAreaRef->GetSoundComponent()->PlaySound(ESoundType::AreaZoneDown);
				AreaSoundPlaying = InAreaRef;
				bIsDownSound = true;
			}
		}
		else
		{
			bIsPlayingSound = true;
			if (bDecrease)
			{
				InAreaRef->GetSoundComponent()->PlaySound(ESoundType::AreaZoneDown);
				AreaSoundPlaying = InAreaRef;
				bIsDownSound = true;
			}
			else
			{
				InAreaRef->GetSoundComponent()->PlaySound(ESoundType::AreaZoneUp);
				AreaSoundPlaying = InAreaRef;
				bIsDownSound = false;
			}
		}
	}
	
	// stop current area effect to apply new one
	GetWorld()->GetTimerManager().ClearTimer(ResetFlameTimer);
	if (Areas.Num() > 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FlameEffectTimer);
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

void UFlameComponent::EndEffect(bool bInIsOneShot, AArea* InAreaRef)
{
	GetWorld()->GetTimerManager().ClearTimer(FlameEffectTimer);
	GetWorld()->GetTimerManager().ClearTimer(ResetFlameTimer);
	
	if (PFECharacter->bIsAlive == false)
	{
		for (FAreaEffect AreaElt : Areas)
		{
			AreaElt.AreaRef->GetSoundComponent()->StopSound();
		}
		Areas.Reset();
		return;
	}

	if (!Areas.IsEmpty())
	{
		int32 IndexToRemove = Areas.IndexOfByPredicate(
			[InAreaRef](const FAreaEffect& Effect)
			{
				return Effect.AreaRef == InAreaRef;
			});

		FAreaEffect CurrentArea; 
		bool bHasValidArea = false; 

		if (IndexToRemove != INDEX_NONE) 
		{
			CurrentArea = Areas[IndexToRemove]; 
			bHasValidArea = true; 
			Areas.RemoveAt(IndexToRemove);
		}

		if (!Areas.IsEmpty()) 
		{			
			FAreaEffect PreviousArea = Areas.Last();

			if (CurrentArea.bDecrease != PreviousArea.bDecrease)
			{
				AreaSoundPlaying->GetSoundComponent()->StopSound();
				if (PreviousArea.bDecrease)
				{
					PreviousArea.AreaRef->GetSoundComponent()->PlaySound(ESoundType::AreaZoneDown);
					bIsDownSound = true;
				}
				else
				{
					PreviousArea.AreaRef->GetSoundComponent()->PlaySound(ESoundType::AreaZoneUp);
					bIsDownSound = false;
				}
				AreaSoundPlaying = PreviousArea.AreaRef;
			}
			
			if (PreviousArea.bIsOneShot)
			{
				SetFlameValue(PreviousArea.Value);
			}
			else
			{
				LaunchEffect(PreviousArea.Value, PreviousArea.Delay);
			}
			return;
		}
		
		if (Areas.IsEmpty() && bHasValidArea) 
		{
			if (AreaSoundPlaying)
			{
				AreaSoundPlaying->GetSoundComponent()->StopSound();
				bIsPlayingSound = false;
			}
			
			if (CurrentFlameValue == 50.f) return;

			// float TickInterval = 0.1f;
			// int32 NumTicks = FMath::CeilToInt(CurrentArea.DelayBeforeNormalFlame / TickInterval);
			// float IncrementValue = (50.f - CurrentFlameValue) / NumTicks;
			//
			// OnChangeFlameValue.Broadcast(false);
			// GetWorld()->GetTimerManager().SetTimer(
			// 	ResetFlameTimer,
			// 	[this, IncrementValue]()
			// 	{
			// 		ResetFlameOverTime(IncrementValue);
			// 	},
			// 	TickInterval,
			// 	true
			// );
		}
	}
}

void UFlameComponent::LaunchEffect(float Value, float Delay)
{
	CheckDeath();
	
	OnChangeFlameValue.Broadcast(true);
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

void UFlameComponent::UpdateFlameValue(float Value)
{
	CheckDeath();
	
	CurrentFlameValue += Value;
	
	UpdateProgressBars();
	UpdateFlameStatus();
	
	if (CurrentFlameValue >= MaxFlameValue || CurrentFlameValue <= 0.f)
	{
		CurrentFlameValue = FMath::Clamp(CurrentFlameValue, 0.f, MaxFlameValue);
		if (CurrentFlameValue <= 0.f)
		{
			PFECharacter->GetSoundComponent()->PlaySound(ESoundType::DeathSmallFlame);
		}
		else
		{
			PFECharacter->GetSoundComponent()->PlaySound(ESoundType::DeathBigFlame);
		}
		OnDeathFlameState.Broadcast(CurrentFlameStatus == EFlameStatus::HIGH);
		
		PFECharacter->GetGameMode()->OnDeath.Broadcast();
	}
}

void UFlameComponent::SetFlameValue(float Value)
{
	CheckDeath();

	PFECharacter->UpdateSmallFlameDelegate.Broadcast(0);
	PFECharacter->UpdateHighFlameDelegate.Broadcast(0);
	
	CurrentFlameValue = Value;

	UpdateProgressBars();
	UpdateFlameStatus();
}

void UFlameComponent::ResetFlameOverTime(float Value)
{
	CheckDeath();

	if ((Value > 0 && CurrentFlameValue >= 49.5f) || (Value < 0 && CurrentFlameValue <= 50.5f))
	{
		OnChangeFlameValue.Broadcast(false);
		GetWorld()->GetTimerManager().ClearTimer(FlameEffectTimer);
		GetWorld()->GetTimerManager().ClearTimer(ResetFlameTimer);
		PFECharacter->UpdateSmallFlameDelegate.Broadcast(0.f);
		PFECharacter->UpdateHighFlameDelegate.Broadcast(0.f);
		CurrentFlameValue = 50.f;
		return;
	}
	
	CurrentFlameValue += Value;
	UpdateProgressBars();
	UpdateFlameStatus();
}

void UFlameComponent::UpdateFlameStatus()
{
	if (CurrentFlameValue < SmallFlameThreshold && CurrentFlameStatus != EFlameStatus::SMALL)
	{
		OnSmallFlame.Broadcast();
		CurrentFlameStatus = EFlameStatus::SMALL;
	}
	if (CurrentFlameValue >= SmallFlameThreshold && CurrentFlameValue < BigFlameThreshold && CurrentFlameStatus != EFlameStatus::NORMAL)
	{
		OnNormalFlame.Broadcast();
		CurrentFlameStatus = EFlameStatus::NORMAL;
	}
	if (CurrentFlameValue >= BigFlameThreshold && CurrentFlameStatus != EFlameStatus::HIGH)
	{
		OnHighFlame.Broadcast();
		CurrentFlameStatus = EFlameStatus::HIGH;
	}
}

void UFlameComponent::CheckDeath()
{
	if (PFECharacter->bIsAlive == false)
	{
		for (FAreaEffect AreaElt : Areas)
		{
			AreaElt.AreaRef->GetSoundComponent()->StopSound();
		}
		bIsPlayingSound = false;
		GetWorld()->GetTimerManager().ClearTimer(FlameEffectTimer);
		GetWorld()->GetTimerManager().ClearTimer(ResetFlameTimer);
		Areas.Reset();
	}
}

void UFlameComponent::UpdateProgressBars()
{
	if (CurrentFlameValue < 50.f)
	{
		float PercentSmall = (50 - CurrentFlameValue) / 50;
		PFECharacter->UpdateSmallFlameDelegate.Broadcast(PercentSmall);
	}
	else
	{
		float PercentBig = (CurrentFlameValue - 50) / 50;
		PFECharacter->UpdateHighFlameDelegate.Broadcast(PercentBig);
	}
}
