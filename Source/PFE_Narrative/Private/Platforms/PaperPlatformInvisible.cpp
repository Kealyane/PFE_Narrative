// Fill out your copyright notice in the Description page of Project Settings.


#include "Platforms/PaperPlatformInvisible.h"
#include "PaperSpriteComponent.h"

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
#if WITH_EDITOR
void APaperPlatformInvisible::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	MirrorPosition = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z-DistanceWithReflexion);
	MirrorRenderComponent->SetWorldLocation(MirrorPosition);
}
#endif

void APaperPlatformInvisible::BeginPlay()
{
	Super::BeginPlay();
	MirrorRenderComponent->SetWorldLocation(MirrorPosition);
}

