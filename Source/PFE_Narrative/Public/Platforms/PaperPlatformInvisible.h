// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "PaperPlatformInvisible.generated.h"

class UPaperSpriteComponent;
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API APaperPlatformInvisible : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	APaperPlatformInvisible();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category="Platform")
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category="Platform")
	TObjectPtr<UPaperSpriteComponent> MainRenderComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="Platform")
	TObjectPtr<UPaperSpriteComponent> MirrorRenderComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"), Category="Platform")
	float DistanceWithReflexion = 100.f;

	#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	#endif
	virtual void BeginPlay() override;

private:
	UPROPERTY(SaveGame)
	FVector MirrorPosition;
};


