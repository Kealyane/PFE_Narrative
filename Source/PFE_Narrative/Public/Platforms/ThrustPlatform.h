// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/SavingSystem/Saveable.h"
#include "GameFramework/Actor.h"
#include "ThrustPlatform.generated.h"

class UUniqueIDComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopPlatformSignature);

UENUM()
enum EThrustPlatformState : uint8
{
	Idle,
	Movement,
	Stop
};

UCLASS()
class PFE_NARRATIVE_API AThrustPlatform : public AActor, public ISaveable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrustPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bPlayerOnBoard;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float SmoothValue = 2.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float StopFrictionValue = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float StartXSpeed = 300.f;

	EThrustPlatformState CurrentState;
	
	// Move
	FVector CurrentVelocity = FVector::ZeroVector;
	FVector TargetVelocity = FVector::ZeroVector;
	FVector NewLocation;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float NextXSpeed;

	// Stop
	FVector Target;
	FVector CurrentPos;
	bool bStop;
	UPROPERTY(BlueprintAssignable)
	FStopPlatformSignature OnStopPlatform;

	TObjectPtr<UUniqueIDComponent> UniqueIDComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void StopPlatform(FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	void AllowMove(float XSpeed, float ZSpeed);

	UFUNCTION(BlueprintCallable)
	void SetZSpeed(float ZSpeed) { TargetVelocity.Z = ZSpeed; }

	UFUNCTION(BlueprintCallable)
	void StopFromHit();

	// SAVE
	UFUNCTION(BlueprintCallable, Category="Save")
	void SetUniqueIDComp(UUniqueIDComponent* InUniqueIDComponent) { UniqueIDComponent = InUniqueIDComponent; }
	
	virtual void OnSave_Implementation(TArray<uint8>& OutData) override;
	virtual void OnLoad_Implementation(const TArray<uint8>& InData) override;
	virtual FString GetActorID_Implementation() override;
};
