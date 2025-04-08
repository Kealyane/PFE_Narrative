// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "PaperPlatformStateChanging.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateChangeDelegate, APaperPlatformStateChanging*, Platform, bool, bIsPlatformOpen);
/**
 * 
 */
UCLASS()
class PFE_NARRATIVE_API APaperPlatformStateChanging : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	APaperPlatformStateChanging();

	UPROPERTY(BlueprintAssignable)
	FStateChangeDelegate StatusChanged;

	void InitPlatform();
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Platform Settings|Initialization")
	bool bAtStartIsOpen;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Platform Settings")
	bool bIsOpen;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Platform Settings", meta = (ToolTip="Delay before disable collider"))
	float DelayBeforeOpening = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Platform Settings", meta = (ToolTip="Delay before enable collider"))
	float DelayBeforeClosing = 0.2f;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void SwitchState();
	
private:
	TObjectPtr<UPrimitiveComponent> PrimitiveComponent;
	UFUNCTION()
	void SwitchCollider();
};
