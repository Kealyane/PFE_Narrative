// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "Components/TimelineComponent.h"
#include "Elevator.generated.h"

class USoundComponent;
class APFECharacter;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API AElevator : public APaperSpriteActor
{
	GENERATED_BODY()
	
public:
	AElevator();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RootElevator;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> ElevatorTriggerBox;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TargetLocationComp;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoxTop;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoxRight;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoxLeft;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundComponent> SoundComponent;
	
	UPROPERTY(EditAnywhere, Category="+Elevator")
	TObjectPtr<UCurveFloat> ElevatorCurve;
	UPROPERTY(EditAnywhere, Category="+Elevator")
	float ElevatorDuration = 4.f;

	FTimeline ElevatorTimeline;
	FVector OriginalLocation;
	FVector TargetLocation;
	TObjectPtr<APFECharacter> PFECharacter;
	bool bElevatorIsMoving;
	FVector PreviousElevatorLocation;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void InitSoundComponent(USoundComponent* InSoundComponent) { SoundComponent = InSoundComponent; }

	UFUNCTION()
	void OnEnterElevator(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnExitElevator(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetBlockCharacter(bool bBlock);

	void ActiveElevator();
	void ReturnElevator();

	UFUNCTION()
	void ElevatorTimelineUpdate(float Value);

	UFUNCTION()
	void OnElevatorArrived();
	
public:	
	virtual void Tick(float DeltaTime) override;
};
