// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PFECharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PFE_Narrative/PFE_NarrativeCharacter.h"

APFECharacter::APFECharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

void APFECharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
	InitGame();
}

void APFECharacter::InitGame()
{
	JumpCount = 0;
	bIsAlive = true;
	bCanMove = true;

	if (MovementComponent)
	{
		SwitchMetrix(SmallFlamesMetrix);
	}
}

void APFECharacter::InitMovementComponent(UCharacterMovementComponent* InMovementComponent)
{
	MovementComponent = InMovementComponent;
	SwitchMetrix(SmallFlamesMetrix);
}

void APFECharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APFECharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APFECharacter::JumpStart);
		// EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APFECharacter::JumpEnd);
		// EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APFECharacter::JumpEnd);
	}
}

void APFECharacter::Move(const FInputActionValue& Value)
{
	MoveValue = Value.Get<float>();

	if (bIsAlive && bCanMove)
	{
		AddMovementInput(DirectionRight, MoveValue);
	}
}

void APFECharacter::JumpStart(const FInputActionValue& Value)
{
	if (bIsAlive && bCanMove && JumpCount < CurrentMetrix.JumpMaxCount)
	{
		LaunchCharacter(DirectionUp * MovementComponent->JumpZVelocity, false, true);
		JumpCount++;
	}
}

void APFECharacter::JumpEnd(const FInputActionValue& Value)
{
	StopJumping();
}

void APFECharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if (PrevMovementMode == MOVE_Falling && MovementComponent->MovementMode == MOVE_Walking)
	{
		JumpCount = 0;
	}
}

void APFECharacter::SwitchMetrix(FCharacterMetrix NewMetrix)
{
	CurrentMetrix = NewMetrix;
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = CurrentMetrix.MoveSpeed;
		MovementComponent->JumpZVelocity = CurrentMetrix.JumpForce;
	}
}

void APFECharacter::SwitchMetrixUI(bool bCheckBoxValue)
{
	if (bCheckBoxValue)
	{
		SwitchMetrix(SmallFlamesMetrix);
	}
	else
	{
		SwitchMetrix(HighFlamesMetrix);
	}
}
