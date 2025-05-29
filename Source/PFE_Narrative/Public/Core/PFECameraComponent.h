// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
	float HorizontalBias = 50.0f;
	
};
