// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFEEnums.h"
#include "Components/ActorComponent.h"
#include "SoundComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PFE_NARRATIVE_API USoundComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USoundComponent();

	UFUNCTION(BlueprintCallable)
	USoundCue* GetSound(ESoundType Type);

	UFUNCTION(BlueprintCallable)
	void PlaySound(ESoundType Type);

	UFUNCTION(BlueprintCallable)
	void StopSound();

	UFUNCTION(BlueprintCallable)
	void StopSmoothSound(float FadeDelay);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAudioComponent> AudioComponent;
	
protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	TMap<ESoundType, USoundCue*> Sounds;
};
