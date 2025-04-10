// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms/PaperPlatformDestructible.h"

#include "Core/PFEGameMode.h"
#include "Core/SoundComponent.h"
#include "Player/PFECharacter.h"

APaperPlatformDestructible::APaperPlatformDestructible()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsDestroyed = false;

	StartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StartPoint"));
	StartPoint->SetupAttachment(RootComponent);
	
	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(RootComponent);
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
	OnActorBeginOverlap.AddDynamic(this, &APaperPlatformDestructible::OnOverlapBegin);

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
		
		if (Hit.ImpactNormal == FVector::DownVector && !bIsDestroyed)
		{
			//UE_LOG(LogTemp, Warning, TEXT("hit"));			

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

void APaperPlatformDestructible::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bIsDestroyed)
	{
		bIsDestroyed = true;
		//UE_LOG(LogTemp, Warning, TEXT("overlap !"));
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
		CheckPlayerInPlatform();
		//UE_LOG(LogTemp, Warning, TEXT("Collision block !"));
	}
	StateChanged.Broadcast(bIsDestroyed);
}

void APaperPlatformDestructible::CheckPlayerInPlatform()
{
	FVector Start = StartPoint->GetComponentLocation();
	FVector End = EndPoint->GetComponentLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(this);

	TArray<FHitResult> HitResults;

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn, 
		FCollisionShape::MakeSphere(SphereRadius),
		QueryParams
	);
	//DrawDebugSphere(GetWorld(), Start, SphereRadius, 12, FColor::Green, false, 1.0f);
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (APFECharacter* HitCharacter = Cast<APFECharacter>(Hit.GetActor()))
			{
				if (APFEGameMode* PFEGameMode = Cast<APFEGameMode>(HitCharacter->GetGameMode()))
				{
					PFEGameMode->LaunchDeathEvent();
				}
			}
		}
	}
}
