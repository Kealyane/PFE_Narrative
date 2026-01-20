// copyright2025 Allen Broquet Lamouche Tamen. All rights reserved.


#include "Platforms/ThrustPlatform.h"

// Sets default values
AThrustPlatform::AThrustPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AThrustPlatform::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EThrustPlatformState::Idle;
}

// Called every frame
void AThrustPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == EThrustPlatformState::Movement)
	{
		CurrentVelocity.X = FMath::FInterpTo(CurrentVelocity.X, TargetVelocity.X, DeltaTime, SmoothValue);
		CurrentVelocity.Z = FMath::FInterpTo(CurrentVelocity.Z, TargetVelocity.Z, DeltaTime, SmoothValue);

		NewLocation = GetActorLocation();
		NewLocation += CurrentVelocity * DeltaTime;
		SetActorLocation(NewLocation);
	}

	if (CurrentState == EThrustPlatformState::Stop)
	{
		CurrentPos = GetActorLocation();

		CurrentVelocity = FMath::VInterpTo(CurrentVelocity, FVector::ZeroVector, DeltaTime, StopFrictionValue);
		NewLocation = CurrentPos + CurrentVelocity * DeltaTime;
		
		if (FVector::DistSquared(NewLocation, Target) < 1.f)
		{
			SetActorLocation(Target);
			CurrentVelocity = FVector::ZeroVector;
			CurrentState = EThrustPlatformState::Idle;
			OnStopPlatform.Broadcast();
			return;
		}

		SetActorLocation(Target);
	}
}

void AThrustPlatform::StopPlatform(FVector TargetLocation)
{
	Target = TargetLocation;
	Target.Y = GetActorLocation().Y;
	bStop = true;
	CurrentState = EThrustPlatformState::Stop;
}

void AThrustPlatform::AllowMove(float XSpeed, float ZSpeed)
{
	TargetVelocity = FVector(XSpeed, 0.0f, ZSpeed);
	CurrentState = EThrustPlatformState::Movement;
}


