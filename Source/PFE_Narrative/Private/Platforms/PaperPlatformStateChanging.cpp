// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms/PaperPlatformStateChanging.h"

#include "Core/PFEGameMode.h"
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
	}
	else
	{
		PrimitiveComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		CheckPlayerInPlatform();
	}
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

