// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Core/SavingSystem/Saveable.h"
#include "Door.generated.h"

class UUniqueIDComponent;
class UCapsuleComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpenDoorDelegate);
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API ADoor : public APaperSpriteActor, public ISaveable
{
	GENERATED_BODY()

public:
	ADoor();

	UPROPERTY(BlueprintAssignable)
	FOpenDoorDelegate OpenDoorDelegate;

	UFUNCTION(BlueprintCallable, Category="Save")
	void SetUniqueIDComp(UUniqueIDComponent* InUniqueIDComponent) { UniqueIDComponent = InUniqueIDComponent; }

	virtual void OnSave_Implementation(TArray<uint8>& OutData) override;
	virtual void OnLoad_Implementation(const TArray<uint8>& InData) override;
	virtual FString GetActorID_Implementation() const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitDoor(bool bInIsOpen);

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Door", meta=(AllowPrivateAccess=true))
	bool bIsOpen;

	TObjectPtr<UUniqueIDComponent> UniqueIDComponent;

	// UPROPERTY(EditAnywhere, Category="Door", meta=(AllowPrivateAccess=true))
	// TObjectPtr<UCapsuleComponent> BlockPathCapsule;
};
