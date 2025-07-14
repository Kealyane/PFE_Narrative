// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraBounds.h"
#include "Components/ActorComponent.h"
#include "PFECameraComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PFE_NARRATIVE_API UPFECameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPFECameraComponent();

protected:
	virtual void BeginPlay() override;

	TObjectPtr<class APFECharacter> PFECharacter;
	TArray<class ACameraBounds*> ActiveBounds;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseDefaultCamera = true;
	
	UFUNCTION(BlueprintCallable)
	void SetPFECharacter(APFECharacter* InCharacter) { PFECharacter = InCharacter; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PFECamera")
	float YLocation = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PFECamera")
	FVector2D DeadZoneSize = FVector2D(100.f, 60.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PFECamera")
	float InterpSpeedSlow = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PFECamera")
	float InterpSpeedFast = 6.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PFECamera")
	float VerticalInterpSpeedFast = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PFECamera")
	float HorizontalBias = 50.0f;

	UFUNCTION(BlueprintCallable)
	void AddBounds(ACameraBounds* InBounds) { if (!ActiveBounds.Contains(InBounds)) ActiveBounds.Add(InBounds); }
	UFUNCTION(BlueprintCallable)
	void RemoveBounds(ACameraBounds* InBounds) { if (ActiveBounds.Contains(InBounds)) ActiveBounds.Remove(InBounds); }

	UFUNCTION(BlueprintCallable)
	void UpdateCameraPosition(float DeltaTime);

private:
	void CameraHalfSize(float& OutHalfWidth, float& OutHalfHeight) const;
	bool FindHighPrioBoundForDirection(EDirection Dir, float& OutValue) const;
	float GetBoundTargetY() const;

	float BIG_VALUE = 100000.f;
	float CurrentY = YLocation;
};
