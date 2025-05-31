// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraBounds.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EDirection : uint8
{
	UP,
	DOWN,
	RIGHT,
	LEFT,
};

UCLASS()
class PFE_NARRATIVE_API ACameraBounds : public AActor
{
	GENERATED_BODY()
	
public:	
	ACameraBounds();
	
	UPROPERTY(EditAnywhere)
	bool bUp;
	UPROPERTY(EditAnywhere)
	bool bDown;
	UPROPERTY(EditAnywhere)
	bool bRight;
	UPROPERTY(EditAnywhere)
	bool bLeft;
	UPROPERTY(EditAnywhere)
	int LayerPriority;
	UPROPERTY(EditAnywhere)
	bool bUpBridge;
	UPROPERTY(EditAnywhere)
	bool bDownBridge;
	UPROPERTY(EditAnywhere)
	bool bRightBridge;
	UPROPERTY(EditAnywhere)
	bool bLeftBridge;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> TriggerZone;

	bool GetBoundingBox(EDirection InDirection, float& OutValue) const;

	
	
protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(VisibleAnywhere, Category = "Camera Bounds")
	TMap<EDirection, float> CachedBounds;

	float BIG_VALUE = 100000.f;
};
