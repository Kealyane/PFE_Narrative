// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Door.h"

#include "Components/CapsuleComponent.h"
#include "Player/PFECharacter.h"

ADoor::ADoor()
{
	bIsOpen = false;
	//BlockPathCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BlockPathCapsule"));
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	InitDoor(false);
	OnActorBeginOverlap.AddDynamic(this, &ADoor::OnOverlapBegin);
}

void ADoor::InitDoor(bool bInIsOpen)
{
	bIsOpen = bInIsOpen;
	// if (bIsOpen)
	// {
	// 	BlockPathCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// }
	// else
	// {
	// 	BlockPathCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// 	BlockPathCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	// }
}

void ADoor::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && !bIsOpen)
	{
		if (APFECharacter* Character = Cast<APFECharacter>(OtherActor))
		{
			if (Character->HasKey())
			{
				bIsOpen = true;
				Character->UseKey();
				OpenDoorDelegate.Broadcast();
//				BlockPathCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			}
		}
	}
}
