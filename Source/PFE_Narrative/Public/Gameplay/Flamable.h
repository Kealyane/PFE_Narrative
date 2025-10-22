// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flamable.generated.h"

class UTimelineComponent;
class ABrasero;
class UPaperSpriteComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActiveFlamableSignature);

UCLASS()
class PFE_NARRATIVE_API AFlamable : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlamable();

	UPROPERTY(BlueprintAssignable)
	FActiveFlamableSignature ActiveFlamable;

	UFUNCTION(BlueprintCallable)
	void OnFlamableDone();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AFlamable> NextFlamable = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> TriggerBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperSpriteComponent> Sprite;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flamable")
	TArray<TObjectPtr<AFlamable>> ConnectedFlamables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flamable")
	float BurningDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flamable")
	TObjectPtr<UMaterialInstance> BurnMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flamable")
	TObjectPtr<ABrasero> BraseoToLit;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flamable")
	bool bHasBurn;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void StartBurn(AFlamable* Flamable);
};
