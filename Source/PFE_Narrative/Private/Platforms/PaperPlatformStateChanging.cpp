// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms/PaperPlatformStateChanging.h"

#include "Components/BoxComponent.h"
#include "Player/PFECharacter.h"

APaperPlatformStateChanging::APaperPlatformStateChanging()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsOpen = false;
}

void APaperPlatformStateChanging::BeginPlay()
{
	Super::BeginPlay();

	PrimitiveComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	
	if (PrimitiveComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimitiveComponent not cast properly"));
	}
	
	OnActorHit.AddDynamic(this, &APaperPlatformStateChanging::OnHit);
}

void APaperPlatformStateChanging::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(APFECharacter::StaticClass()))
	{
		if (Hit.ImpactNormal == FVector::DownVector)
		{
			UE_LOG(LogTemp, Warning, TEXT("Collision avec le joueur !"));			

			bIsOpen = true;
			FTimerHandle PlayerOnTimer;
			FTimerHandle SwitchTimer;

			
			GetWorld()->GetTimerManager().SetTimer(PlayerOnTimer, this,
				&APaperPlatformStateChanging::SwitchCollisionPreset, DelayWhenPlayerOn, false);

			GetWorld()->GetTimerManager().SetTimer(SwitchTimer,
				[this]()
				{
					bIsOpen = false;
					SwitchCollisionPreset();
				}, DelayBeforeSwitch+DelayWhenPlayerOn, false);
		}
	}
}

void APaperPlatformStateChanging::SwitchCollisionPreset()
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
	StateChanged.Broadcast(bIsOpen);
}


