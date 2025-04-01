// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Door.generated.h"

class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenDoorDelegate);
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API ADoor : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	ADoor();

	UPROPERTY(BlueprintAssignable)
	FOpenDoorDelegate OpenDoorDelegate;

protected:
	virtual void BeginPlay() override;

	void InitDoor(bool bInIsOpen);

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess=true))
	bool bIsOpen;

	// UPROPERTY(EditAnywhere, Category="Door", meta=(AllowPrivateAccess=true))
	// TObjectPtr<UCapsuleComponent> BlockPathCapsule;
};
