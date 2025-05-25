// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Elevator.h"

#include "Components/BoxComponent.h"
#include "Core/SoundComponent.h"
#include "Player/PFECharacter.h"

class APFECharacter;

AElevator::AElevator()
{
	PrimaryActorTick.bCanEverTick = true;

	RootElevator = CreateDefaultSubobject<USceneComponent>(TEXT("RootElevator"));
	RootComponent = RootElevator;

	ElevatorTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ElevatorTriggerBox"));
	ElevatorTriggerBox->SetupAttachment(RootElevator);
	ElevatorTriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ElevatorTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AElevator::OnEnterElevator);
	ElevatorTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AElevator::OnExitElevator);
	
	TargetLocationComp = CreateDefaultSubobject<USceneComponent>(TEXT("Target Location"));
	TargetLocationComp->SetupAttachment(RootElevator);

	BoxTop = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTop"));
	BoxTop->SetupAttachment(RootElevator);
	
	BoxRight = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxRight"));
	BoxRight->SetupAttachment(RootElevator);
	
	BoxLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxLeft"));
	BoxLeft->SetupAttachment(RootElevator);
}

void AElevator::BeginPlay()
{
	Super::BeginPlay();
	
	if (ElevatorCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("ElevatorTimelineUpdate"));
		ElevatorTimeline.AddInterpFloat(ElevatorCurve, ProgressFunction);

		FOnTimelineEvent OnFinishEvent;
		OnFinishEvent.BindUFunction(this, FName("OnElevatorArrived"));
		ElevatorTimeline.SetTimelineFinishedFunc(OnFinishEvent);

		ElevatorTimeline.SetLooping(false);
		ElevatorTimeline.SetTimelineLength(ElevatorDuration);
		ElevatorTimeline.SetPlayRate(1.0f / ElevatorDuration);
	}
	OriginalLocation = GetActorLocation();
	SetBlockCharacter(false);
	PreviousElevatorLocation = GetActorLocation();
}

void AElevator::OnEnterElevator(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APFECharacter* CharacterFound = Cast<APFECharacter>(OtherActor))
	{
		if (!PFECharacter)
		{
			PFECharacter = CharacterFound;
			
			FTimerHandle ActiveHandle;
			GetWorld()->GetTimerManager().SetTimer(ActiveHandle, this, &AElevator::ActiveElevator, 1.f, false);
		}
	}
}

void AElevator::OnExitElevator(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (Cast<APFECharacter>(OtherActor))
	{
		if (PFECharacter)
		{
			PFECharacter = nullptr;
			FTimerHandle ActiveHandle;
			GetWorld()->GetTimerManager().SetTimer(ActiveHandle, this, &AElevator::ReturnElevator, 0.5f, false);
		}
	}
}

void AElevator::SetBlockCharacter(bool bBlock)
{
	BoxTop->SetCollisionResponseToChannel(ECC_Pawn, bBlock ? ECR_Block : ECR_Ignore);
	BoxRight->SetCollisionResponseToChannel(ECC_Pawn, bBlock ? ECR_Block : ECR_Ignore);
	BoxLeft->SetCollisionResponseToChannel(ECC_Pawn, bBlock ? ECR_Block : ECR_Ignore);
}

void AElevator::ActiveElevator()
{
	PreviousElevatorLocation = GetActorLocation();
	SetBlockCharacter(true);
	bElevatorIsMoving = true;
	TargetLocation = TargetLocationComp->GetComponentLocation();
	ElevatorTimeline.PlayFromStart();
	SoundComponent->PlaySound(ESoundType::Lift);
}

void AElevator::ReturnElevator()
{
	PreviousElevatorLocation = GetActorLocation();
	SetBlockCharacter(true);
	bElevatorIsMoving = false;
	ElevatorTimeline.ReverseFromEnd();
	SoundComponent->PlaySound(ESoundType::Lift);
}

void AElevator::ElevatorTimelineUpdate(float Value)
{
	FVector NewLocation = FMath::Lerp(OriginalLocation, TargetLocation, Value);
	SetActorLocation(NewLocation);
}

void AElevator::OnElevatorArrived()
{
	SetBlockCharacter(false);
	PreviousElevatorLocation = GetActorLocation();
	SoundComponent->StopSound();
}

void AElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PFECharacter && bElevatorIsMoving)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Delta = CurrentLocation - PreviousElevatorLocation;
		FVector VerticalDelta(0.f, 0.f, Delta.Z);

		PFECharacter->AddActorWorldOffset(VerticalDelta, true);

		PreviousElevatorLocation = CurrentLocation;
	}
	ElevatorTimeline.TickTimeline(DeltaTime);
}
