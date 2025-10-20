// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms/PaperPlatformStateChanging.h"

#include "Kismet/GameplayStatics.h"
#include "Player/PFECharacter.h"

APaperPlatformStateChanging::APaperPlatformStateChanging()
{
	PrimaryActorTick.bCanEverTick = false;
	bAtStartIsOpen = false;
	bIsOpen = false;
	
	StartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StartPoint"));
	StartPoint->SetupAttachment(RootComponent);
	
	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(RootComponent);
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

	InitPlatform();
}

void APaperPlatformStateChanging::SwitchState()
{
	bIsOpen = !bIsOpen;
	
	StatusChanged.Broadcast(this, bIsOpen);

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
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
	else
	{
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		CheckPlayerInPlatform();
	}
}

void APaperPlatformStateChanging::Block()
{
	PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

void APaperPlatformStateChanging::Ignore()
{
	PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void APaperPlatformStateChanging::CheckPlayerInPlatform()
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

// 	DrawDebugCapsule(
// 	GetWorld(),
// 	(Start + End) * 0.5f,                          // centre
// 	(FVector::Dist(Start, End) * 0.5f),            // demi-longueur
// 	SphereRadius,                                  // rayon
// 	FRotationMatrix::MakeFromZ(End - Start).ToQuat(), // orientation
// 	FColor::Yellow,
// 	false,
// 	2.0f
// );

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
					Ignore();
					LaunchVelocity = FVector(-100.f, 0.f, -800.f);
					GetWorld()->GetTimerManager().SetTimer(BlockTimer, this,
						&APaperPlatformStateChanging::Block, 0.2f, false);
				}
				else
				{
					Ignore();
					LaunchVelocity = FVector(100.f, 0.f, 800.f);
					GetWorld()->GetTimerManager().SetTimer(BlockTimer, this,
						&APaperPlatformStateChanging::Block, 0.2f, false);
				}
				HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
			}
		}
	}
}

