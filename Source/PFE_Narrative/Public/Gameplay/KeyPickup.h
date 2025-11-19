// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Core/SavingSystem/Saveable.h"
#include "KeyPickup.generated.h"

class UUniqueIDComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadDoneDelegate, bool, bTaken);
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API AKeyPickup : public APaperSpriteActor, public ISaveable
{
	GENERATED_BODY()

public:
	AKeyPickup();

	virtual void OnSave_Implementation(TArray<uint8>& OutData) override;
	virtual void OnLoad_Implementation(const TArray<uint8>& InData) override;
	virtual FString GetActorID_Implementation() const override;

	UFUNCTION(BlueprintCallable, Category="Save")
	void SetUniqueIDComp(UUniqueIDComponent* InUniqueIDComponent) { UniqueIDComponent = InUniqueIDComponent; }
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION()
	void SetKeyVisibility(bool Visible);
	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UPROPERTY(BlueprintAssignable)
	FLoadDoneDelegate LoadDone;

private:
	TObjectPtr<UUniqueIDComponent> UniqueIDComponent;
	bool bHasBeenTaken;
};
