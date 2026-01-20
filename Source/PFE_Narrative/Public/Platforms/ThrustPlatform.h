// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrustPlatform.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopPlatformSignature);

UENUM()
enum EThrustPlatformState : uint8
{
	Idle,
	Movement,
	Stop
};

UCLASS()
class PFE_NARRATIVE_API AThrustPlatform : public AActor
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

	EThrustPlatformState CurrentState;
	
	// Move
	FVector CurrentVelocity = FVector::ZeroVector;
	FVector TargetVelocity = FVector::ZeroVector;
	FVector NewLocation;

	// Stop
	FVector Target;
	FVector CurrentPos;
	bool bStop;
	UPROPERTY(BlueprintAssignable)
	FStopPlatformSignature OnStopPlatform;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void StopPlatform(FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	void AllowMove(float XSpeed, float ZSpeed);
};
