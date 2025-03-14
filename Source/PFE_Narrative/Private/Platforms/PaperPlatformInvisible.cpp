// Fill out your copyright notice in the Description page of Project Settings.


#include "PaperSpriteComponent.h"
#include "Platforms/PaperPlatformInvisible.h"

APaperPlatformInvisible::APaperPlatformInvisible()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MainRenderComponent = GetRenderComponent();
	MainRenderComponent->SetupAttachment(Root);
	
	MirrorRenderComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MirrorRenderComponent"));
	MirrorRenderComponent->SetupAttachment(Root);
	
	MirrorPosition = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z-DistanceWithReflexion);
	MirrorRenderComponent->SetWorldLocation(MirrorPosition);
}

void APaperPlatformInvisible::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	MirrorPosition = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z-DistanceWithReflexion);
	MirrorRenderComponent->SetWorldLocation(MirrorPosition);
}

void APaperPlatformInvisible::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	MirrorPosition = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z-DistanceWithReflexion);
	MirrorRenderComponent->SetWorldLocation(MirrorPosition);
}

void APaperPlatformInvisible::BeginPlay()
{
	Super::BeginPlay();
	MirrorRenderComponent->SetWorldLocation(MirrorPosition);
}

