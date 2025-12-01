// copyright2025  Allen Broquet Lamouche Tamen. All rights reserved.


#include "Gameplay/Brasero.h"

#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "Components/SphereComponent.h"
#include "Core/PFEGameInstance.h"
#include "Core/SavingSystem/UniqueIDComponent.h"
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

// SAVE - LOAD
void ABrasero::OnSave_Implementation(TArray<uint8>& OutData)
{
	FMemoryWriter Writer(OutData);
	Writer << bIsOn;
}

void ABrasero::OnLoad_Implementation(const TArray<uint8>& InData)
{
	FMemoryReader Reader(InData);
	Reader << bIsOn;

	//bIsOn ? TurnOnBrasero() : TurnOffBrasero();
	InitializeBraseroLoad();
}

FString ABrasero::GetActorID_Implementation() const
{
	return UniqueIDComponent->ActorID;
}
// -----------------------

void ABrasero::BeginPlay()
{
	Super::BeginPlay();

	if (UPFEGameInstance* PFE_GI = Cast<UPFEGameInstance>(GetWorld()->GetGameInstance()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("id : %s"), *UniqueIDComponent->ActorID)
		PFE_GI->RegisterToSave(this);
	}
	
	FlameFlipbook->SetHiddenInGame(true);
	ExplosionFlipbook->SetHiddenInGame(true);
}

void ABrasero::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UPFEGameInstance* PFE_GI = Cast<UPFEGameInstance>(GetWorld()->GetGameInstance()))
	{
		PFE_GI->UnregisterFromSave(this);
	}
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

void ABrasero::InitializeBraseroLoad()
{
	if (bIsOn)
	{
		FlameFlipbook->SetHiddenInGame(false);
		FlameFlipbook->SetLooping(true);
		FlameFlipbook->PlayFromStart();
	}
	else
	{
		FlameFlipbook->SetHiddenInGame(true);
		FlameFlipbook->Stop();
	}
}



