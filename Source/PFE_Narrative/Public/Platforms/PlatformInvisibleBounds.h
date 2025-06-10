// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformInvisibleBounds.generated.h"

UCLASS()
class PFE_NARRATIVE_API APlatformInvisibleBounds : public AActor
{
	GENERATED_BODY()
	
public:	
	APlatformInvisibleBounds();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	bool bIsReflexionHorizontal = true;

	UFUNCTION()
	void EnterInvisibleArea(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ExitInvisibleArea(AActor* OverlappedActor, AActor* OtherActor);
};
