// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms/PaperPlatformDestructible.h"

#include "Core/SoundComponent.h"
#include "Player/PFECharacter.h"

APaperPlatformDestructible::APaperPlatformDestructible()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsDestroyed = false;
}

void APaperPlatformDestructible::InitPlatform()
{
	bIsDestroyed = false;
	PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	StateChanged.Broadcast(bIsDestroyed);
}

void APaperPlatformDestructible::BeginPlay()
{
	Super::BeginPlay();

	PrimitiveComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	
	if (PrimitiveComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PrimitiveComponent not cast properly"));
	}
	
	OnActorHit.AddDynamic(this, &APaperPlatformDestructible::OnHit);

	InitPlatform();
}

void APaperPlatformDestructible::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(APFECharacter::StaticClass()))
	{
		if (bShowDebug)
		{
			// Debug : Draw Impact Normal (Red - length : 50)
			DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (Hit.ImpactNormal * 50), FColor::Red, false, 3.0f, 0, 2.0f);
			DrawDebugDirectionalArrow(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (Hit.ImpactNormal * 50), 25.0f, FColor::Red, false, 3.0f, 0, 2.0f);
			UE_LOG(LogTemp, Warning, TEXT("Hit Normal: %s"), *Hit.ImpactNormal.ToString());

			// Debug : Draw Down Vector (Blue - length 25)
			DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (FVector::DownVector * 25), FColor::Blue, false, 3.0f, 0, 2.0f);
			DrawDebugDirectionalArrow(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (FVector::DownVector * 25), 10.0f, FColor::Blue, false, 3.0f, 0, 2.0f);
			UE_LOG(LogTemp, Warning, TEXT("FVector::DownVector: %s"), *FVector::DownVector.ToString());
		}
		
		if (Hit.ImpactNormal == FVector::DownVector)
		{
			UE_LOG(LogTemp, Warning, TEXT("Collision avec le joueur !"));			

			bIsDestroyed = true;
			FTimerHandle PlayerOnTimer;
			FTimerHandle SwitchTimer;

			
			GetWorld()->GetTimerManager().SetTimer(PlayerOnTimer, this,
				&APaperPlatformDestructible::SwitchCollisionPreset, DelayWhenPlayerOn, false);

			GetWorld()->GetTimerManager().SetTimer(SwitchTimer,
				[this]()
				{
					bIsDestroyed = false;
					SwitchCollisionPreset();
				}, DelayBeforeSwitch+DelayWhenPlayerOn, false);
		}
	}
}

void APaperPlatformDestructible::SwitchCollisionPreset()
{
	if (PrimitiveComponent == nullptr) return;
	
	if (bIsDestroyed)
	{
		SoundComponent->PlaySound(ESoundType::PlatformDestructible);
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		//UE_LOG(LogTemp, Warning, TEXT("Collision Ignore !"));
	}
	else
	{
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		//UE_LOG(LogTemp, Warning, TEXT("Collision block !"));
	}
	StateChanged.Broadcast(bIsDestroyed);
}
