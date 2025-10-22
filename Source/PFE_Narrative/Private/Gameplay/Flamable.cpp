// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.


#include "Gameplay/Flamable.h"

#include "PaperSpriteComponent.h"
#include "Components/BoxComponent.h"
#include "Gameplay/Brasero.h"
#include "Player/FlameComponent.h"
#include "Player/PFECharacter.h"

AFlamable::AFlamable()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	TriggerBox->SetupAttachment(Root);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFlamable::OnTriggerBeginOverlap);
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	CollisionBox->SetupAttachment(Root);

	Sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(Root);
}

void AFlamable::BeginPlay()
{
	Super::BeginPlay();
	
	bHasBurn = false;

	if (ConnectedFlamables.Num() > 0)
	{
		NextFlamable = ConnectedFlamables[0];

		for (int i = 0; i < ConnectedFlamables.Num(); i++)
		{
			ConnectedFlamables[i]->TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			ConnectedFlamables[i]->CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			
			if (i+1 < ConnectedFlamables.Num())
			{
				ConnectedFlamables[i]->NextFlamable = ConnectedFlamables[i+1];
			}
		}

		ConnectedFlamables.Last()->NextFlamable = nullptr;
	}
}

void AFlamable::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APFECharacter* Character = Cast<APFECharacter>(OtherActor))
	{
		if (Character->GetFlameComponent()->GetFlameStatus() == EFlameStatus::HIGH)
		{
			StartBurn(this);
		}
	}
}

void AFlamable::StartBurn(AFlamable* Flamable)
{
	if (Flamable == nullptr || Flamable->bHasBurn)
	{
		return;
	}
	
	Flamable->bHasBurn = true;
	Flamable->ActiveFlamable.Broadcast();
	Flamable->TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	Flamable->CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	UMaterialInstanceDynamic* BurnMatDynamic = UMaterialInstanceDynamic::Create(BurnMaterialInstance, this);
	Flamable->Sprite->SetMaterial(0, BurnMatDynamic);
	BurnMatDynamic->SetScalarParameterValue("BurningTime", GetWorld()->GetTimeSeconds());
}

void AFlamable::OnFlamableDone()
{
	Sprite->SetVisibility(false);
	
	if (BraseoToLit != nullptr) BraseoToLit->TurnOnBrasero();

	if (NextFlamable != nullptr) StartBurn(NextFlamable);
}

