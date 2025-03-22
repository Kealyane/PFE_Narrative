// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms/PaperPlatformStateChanging.h"

#include "Kismet/GameplayStatics.h"
#include "Player/PFECharacter.h"



APaperPlatformStateChanging::APaperPlatformStateChanging()
{
	PrimaryActorTick.bCanEverTick = false;
	bAtStartIsOpen = false;
	bIsOpen = false;
}

void APaperPlatformStateChanging::InitPlatform()
{
	bIsOpen = bAtStartIsOpen;
	SwitchCollider();
}

void APaperPlatformStateChanging::BeginPlay()
{
	Super::BeginPlay();
	
	PrimitiveComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	
	if (PrimitiveComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimitiveComponent not cast properly"));
	}

	APFECharacter* PFECharacter = Cast<APFECharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PFECharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to cast character to PFECharacter"));
	}
	else
	{
		PFECharacter->JumpDelegate.AddDynamic(this, &APaperPlatformStateChanging::SwitchState);
	}
}

void APaperPlatformStateChanging::SwitchState()
{
	bIsOpen = !bIsOpen;
	
	StatusChanged.Broadcast(bIsOpen);

	float DelayToApply = bIsOpen ? DelayBeforeClosing : DelayBeforeOpening;
	
	FTimerHandle SwitchStateTimer;
	GetWorld()->GetTimerManager().SetTimer(SwitchStateTimer, this,
		&APaperPlatformStateChanging::SwitchCollider, DelayToApply, false);
}

void APaperPlatformStateChanging::SwitchCollider()
{
	if (PrimitiveComponent == nullptr) return;
	
	if (bIsOpen)
	{
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		UE_LOG(LogTemp, Warning, TEXT("Collision Ignore !"));
	}
	else
	{
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		UE_LOG(LogTemp, Warning, TEXT("Collision block !"));
	}
}

