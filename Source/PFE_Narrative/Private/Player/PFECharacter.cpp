// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PFECharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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
}

void APFECharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APFECharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APFECharacter::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APFECharacter::JumpEnd);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APFECharacter::JumpEnd);
	}
}

void APFECharacter::Move(const FInputActionValue& Value)
{
	float MoveValue = Value.Get<float>();

	if (bIsAlive && bCanMove)
	{
		FVector Direction = FVector(1.f, 0.f, 0.f);
		AddMovementInput(Direction, MoveValue);
	}
}

void APFECharacter::JumpStart(const FInputActionValue& Value)
{
	if (bIsAlive && bCanMove)
	{
		Jump();
	}
}

void APFECharacter::JumpEnd(const FInputActionValue& Value)
{
	StopJumping();
}
