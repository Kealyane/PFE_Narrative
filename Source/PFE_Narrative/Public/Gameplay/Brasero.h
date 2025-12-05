// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/SavingSystem/Saveable.h"
#include "GameFramework/Actor.h"
#include "Brasero.generated.h"

class UUniqueIDComponent;
class UPaperSpriteComponent;
class USphereComponent;
class UPaperFlipbookComponent;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnOnSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnOffSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWrongFlameSmallSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWrongFlameBigSignature);

UCLASS()
class PFE_NARRATIVE_API ABrasero : public AActor, public ISaveable
{
	GENERATED_BODY()
	
public:
	ABrasero();

	UFUNCTION(BlueprintCallable)
	void HideBrasero(bool Hidden);

	UFUNCTION(BlueprintCallable, Category="Save")
	void SetUniqueIDComp(UUniqueIDComponent* InUniqueIDComponent) { UniqueIDComponent = InUniqueIDComponent; }

	virtual void OnSave_Implementation(TArray<uint8>& OutData) override;
	virtual void OnLoad_Implementation(const TArray<uint8>& InData) override;
	virtual FString GetActorID_Implementation() override;

	/**
	* Turn on flame brasero, increment door number and check if the door can be open
	*/
	void TurnOnBrasero();

protected:
	
	TObjectPtr<UUniqueIDComponent> UniqueIDComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperSpriteComponent> Sprite;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> FlameFlipbook;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereTrigger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperFlipbookComponent> ExplosionFlipbook;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsOn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Brasero")
	TObjectPtr<class ADoorFlame> AffiliatedDoor;

	UPROPERTY(BlueprintAssignable)
	FTurnOnSignature TurnOnEvent;
	UPROPERTY(BlueprintAssignable)
	FTurnOffSignature TurnOffEvent;
	UPROPERTY(BlueprintAssignable)
	FWrongFlameSmallSignature WrongFlameSmallEvent;
	UPROPERTY(BlueprintAssignable)
	FWrongFlameBigSignature WrongFlameBigEvent;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	/**
	* Turn off flame brasero, decrement door number and close the door
	*/
	UFUNCTION()
	void TurnOffBrasero();
	/**
	* Show wrong flame
	*/
	UFUNCTION()
	void WrongFlameSmall();
	/**
	* Launch brasero explosion, invisible for a certain time
	* BP : use HideBrasero
	*/
	UFUNCTION()
	void WrongFlameBig();

	void InitializeBraseroLoad();
};
