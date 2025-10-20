// copyright2025  Allen Broquet Lamouche Tamen. All rights reserved.


#include "Gameplay/Brasero.h"

#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "Components/SphereComponent.h"
#include "Gameplay/DoorFlame.h"

ABrasero::ABrasero()
{
	PrimaryActorTick.bCanEverTick = false;

	bIsOn = false;
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteBrasero"));
	Sprite->SetupAttachment(Root);

	FlameFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlameFlipbook"));
	FlameFlipbook->SetupAttachment(Root);
	
	SphereTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
	SphereTrigger->SetupAttachment(Root);

	ExplosionFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ExplosionFlipbook"));
	ExplosionFlipbook->SetupAttachment(Root);
	ExplosionFlipbook->SetLooping(false);
	ExplosionFlipbook->Stop();
}

void ABrasero::BeginPlay()
{
	Super::BeginPlay();
	FlameFlipbook->SetHiddenInGame(true);
	ExplosionFlipbook->SetHiddenInGame(true);
	bIsOn = false;
}

void ABrasero::TurnOnBrasero()
{
	bIsOn = true;
	
	FlameFlipbook->SetHiddenInGame(false);
	FlameFlipbook->SetLooping(true);
	FlameFlipbook->PlayFromStart();

	if (!AffiliatedDoor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TurnOn::No Door Affiliated to Brasero %s"), *GetName());
	}
	else
	{
		AffiliatedDoor->IncrementBraseroOn();
		AffiliatedDoor->CheckNumBraseros();
	}

	TurnOnEvent.Broadcast();
}

void ABrasero::TurnOffBrasero()
{
	bIsOn = false;
	
	FlameFlipbook->SetHiddenInGame(true);
	FlameFlipbook->Stop();

	if (!AffiliatedDoor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TurnOff::No Door Affiliated to Brasero %s"), *GetName());
	}
	else
	{
		AffiliatedDoor->DecrementBraseroOn();
		AffiliatedDoor->CheckNumBraseros();
	}
	
	TurnOffEvent.Broadcast();
}

void ABrasero::WrongFlameSmall()
{
	WrongFlameSmallEvent.Broadcast();
}

void ABrasero::WrongFlameBig()
{
	ExplosionFlipbook->SetHiddenInGame(false);
	ExplosionFlipbook->PlayFromStart();

	WrongFlameBigEvent.Broadcast();	// HideBrasero in BP
}

void ABrasero::HideBrasero(bool Hidden)
{
	Sprite->SetHiddenInGame(Hidden);
	if (bIsOn) FlameFlipbook->SetHiddenInGame(Hidden);
	
	if (Hidden)
		SphereTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	else
		SphereTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}



