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
	//PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
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
	
	//OnActorHit.AddDynamic(this, &APaperPlatformDestructible::OnHit);
	//OnActorBeginOverlap.AddDynamic(this, &APaperPlatformDestructible::OnOverlapBegin);

	InitPlatform();
}

void APaperPlatformDestructible::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HitSwitchTimer);
		World->GetTimerManager().ClearTimer(HitPlayerOnTimer);
		World->GetTimerManager().ClearTimer(OverlapSwitchTimer);
		World->GetTimerManager().ClearTimer(OverlapPlayerOnTimer);
	}
}

// void APaperPlatformDestructible::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse,
// 	const FHitResult& Hit)
// {
// 	if (OtherActor && OtherActor->IsA(APFECharacter::StaticClass()))
// 	{
// 		// if (bShowDebug)
// 		// {
// 		// 	// Debug : Draw Impact Normal (Red - length : 50)
// 		// 	DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (Hit.ImpactNormal * 50), FColor::Red, false, 3.0f, 0, 2.0f);
// 		// 	DrawDebugDirectionalArrow(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (Hit.ImpactNormal * 50), 25.0f, FColor::Red, false, 3.0f, 0, 2.0f);
// 		// 	UE_LOG(LogTemp, Warning, TEXT("Hit Normal: %s"), *Hit.ImpactNormal.ToString());
// 		//
// 		// 	// Debug : Draw Down Vector (Blue - length 25)
// 		// 	DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (FVector::DownVector * 25), FColor::Blue, false, 3.0f, 0, 2.0f);
// 		// 	DrawDebugDirectionalArrow(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + (FVector::DownVector * 25), 10.0f, FColor::Blue, false, 3.0f, 0, 2.0f);
// 		// 	UE_LOG(LogTemp, Warning, TEXT("FVector::DownVector: %s"), *FVector::DownVector.ToString());
// 		// }
// 		
// 		if (Hit.ImpactNormal == FVector::DownVector && !bIsDestroyed)
// 		{
// 			bIsDestroyed = true;
// 			
// 			GetWorld()->GetTimerManager().SetTimer(HitPlayerOnTimer, this,
// 				&APaperPlatformDestructible::SwitchCollisionPreset, DelayWhenPlayerOn, false);
// 			
// 			GetWorld()->GetTimerManager().SetTimer(HitSwitchTimer,
// 				[this]()
// 				{
// 					bIsDestroyed = false;
// 					SwitchCollisionPreset();
// 				}, DelayBeforeSwitch+DelayWhenPlayerOn, false);
// 		}
// 	}
// }

// void APaperPlatformDestructible::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
// {
// 	if (!bIsDestroyed)
// 	{
// 		bIsDestroyed = true;
// 		
// 		GetWorld()->GetTimerManager().SetTimer(OverlapPlayerOnTimer, this,
// 	&APaperPlatformDestructible::SwitchCollisionPreset, DelayWhenPlayerOn, false);
//
// 		GetWorld()->GetTimerManager().SetTimer(OverlapSwitchTimer,
// 			[this]()
// 			{
// 				bIsDestroyed = false;
// 				SwitchCollisionPreset();
// 			}, DelayBeforeSwitch+DelayWhenPlayerOn, false);
// 	}
// }

void APaperPlatformDestructible::LaunchTimersToDestroyPlatform()
{
	if (!bIsDestroyed)
	{
		bIsDestroyed = true;
		
		GetWorld()->GetTimerManager().SetTimer(OverlapPlayerOnTimer, this,
	&APaperPlatformDestructible::SwitchCollisionPreset, DelayWhenPlayerOn, false);

		GetWorld()->GetTimerManager().SetTimer(OverlapSwitchTimer,
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
		// PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		// PrimitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
	// traitement en BP
	// else
	// {
	// 	PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	// 	PrimitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	// 	CheckPlayerInPlatform();
	// }
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

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (APFECharacter* HitCharacter = Cast<APFECharacter>(Hit.GetActor()))
			{
				FVector LaunchVelocity;
				FTimerHandle BlockTimer;
				if (HitCharacter->GetActorLocation().Z < GetActorLocation().Z)
				{
					PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
					LaunchVelocity = FVector(-100.f, 0.f, -800.f);
					GetWorld()->GetTimerManager().SetTimer(BlockTimer, this,
						&APaperPlatformDestructible::Block, 0.2f, false);
				}
				else
				{
					PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
					LaunchVelocity = FVector(100.f, 0.f, 800.f);
					GetWorld()->GetTimerManager().SetTimer(BlockTimer, this,
						&APaperPlatformDestructible::Block, 0.2f, false);
				}
				HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
			}
		}
	}
}

void APaperPlatformDestructible::Block()
{
	PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}
