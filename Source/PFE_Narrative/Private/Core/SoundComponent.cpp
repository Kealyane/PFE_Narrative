// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SoundComponent.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

USoundComponent::USoundComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
}

USoundCue* USoundComponent::GetSound(ESoundType Type)
{
	return Sounds[Type];
}

void USoundComponent::PlaySound(ESoundType Type)
{
	if (Sounds.Contains(Type))
	{
		AudioComponent->SetSound(Sounds[Type]);
		AudioComponent->Play();
	}
}


// Called when the game starts
void USoundComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

