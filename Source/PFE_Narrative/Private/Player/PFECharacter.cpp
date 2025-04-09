// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PFECharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/PFEGameMode.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PFE_Narrative/PFE_NarrativeCharacter.h"
#include "Math/Vector.h"
#include "Player/FlameComponent.h"
#include "Core/SoundComponent.h"

APFECharacter::APFECharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
	InitGameMode();

	if (FlameComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Flame Component empty"));
	}
}

void APFECharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsInReflexionArea)
	{
		float Distance = FMath::Abs(ReflexionAreaGround - GetActorLocation().Z);
		FVector NewLocation = FVector(ReflexionPlaneLocation.X, ReflexionPlaneLocation.Y, -(Distance*2));
		ReflexionPlane->SetRelativeLocation(NewLocation);
	}

	if (bShowDebug)
	{
		//if (bIsOnGround) PrintOnScreen("Is On Ground");
		if (bIsDashing) PrintOnScreen("Is Dashing");
		if (bIsNearWall) PrintOnScreen("Is Near Wall");
		if (bIsGrabbingWall) PrintOnScreen("Is Grabbing Wall");
		if (bIsDoingWallJump) PrintOnScreen("Is Doing Wall Jump");
		if (bIsJumping) PrintOnScreen("Is Jumping");
		if (bBlockHorizontalInput) PrintOnScreen("Block Horizontal Input");
		if (MovementComponent->MovementMode == MOVE_Falling) PrintOnScreen("Movement Falling");
	}
}

void APFECharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		FVector Start = GetActorLocation();
		FVector End = Start + GetActorForwardVector() * 100.0f;

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
		if (bHit)
		{
			float DotValue = FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector);
			
			if (FMath::Abs(DotValue) < DotThreashold) // is a Wall
			{
				if (HitResult.ImpactNormal.X != MoveValue)
				{
					bIsNearWall = true;
					WallNormal = HitResult.ImpactNormal;
				}
			}
		}

		// FColor LineColor = bHit ? FColor::Green : FColor::Red;
		// DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);
		//
		// if (bHit)
		// {
		// 	DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 40.0f, FColor::Blue, false, 2.0f);
		// }
	}
}

void APFECharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bIsNearWall || bIsGrabbingWall)
	{
		bIsNearWall = false;
		WallGrabEnd();
	}
}

void APFECharacter::InitGame()
{
	JumpCount = 0;
	bIsAlive = true;
	bCanMove = true;
	bCanDash = true;
	bIsInReflexionArea = false;
	bBlockHorizontalInput = false;

	if (MovementComponent)
	{
		SwitchMetrix(SmallFlamesMetrix);
	}
	if (FlameComponent)
	{
		FlameComponent->InitFlame();
	}
}

void APFECharacter::InitGameMode()
{
	PFEGameMode = Cast<APFEGameMode>(UGameplayStatics::GetGameMode(this));
	check(PFEGameMode);
	PFEGameMode->OnDeath.AddDynamic(this, &APFECharacter::LaunchRespawn);
}

void APFECharacter::FlipCharacter(float Direction)
{
	if (Direction < 0.0)
	{
		SetActorRotation(LeftOrientation);
		GetController()->SetControlRotation(LeftOrientation);
	}
	else
	{
		SetActorRotation(RightOrientation);
		GetController()->SetControlRotation(RightOrientation);
	}
}

void APFECharacter::InitMovementComponent(UCharacterMovementComponent* InMovementComponent)
{
	MovementComponent = InMovementComponent;
	SwitchMetrix(SmallFlamesMetrix);
}


void APFECharacter::InitCapsuleComponent(UCapsuleComponent* InCapsuleComponent)
{
	CapsuleComponent = InCapsuleComponent;
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &APFECharacter::OnOverlapBegin);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &APFECharacter::OnOverlapEnd);
}

void APFECharacter::InitReflexionPlane(UStaticMeshComponent* InReflexionPlane)
{
	ReflexionPlane = InReflexionPlane;
	ReflexionPlaneLocation = ReflexionPlane->GetRelativeLocation();
	ReflexionPlane->SetHiddenInGame(true);
}

void APFECharacter::InitFlameComponent(UFlameComponent* InFlameComponent)
{
	FlameComponent = InFlameComponent;
	FlameComponent->InitFlame();
}


void APFECharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APFECharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APFECharacter::MoveEnd);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APFECharacter::JumpStart);
		// EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APFECharacter::JumpEnd);
		// EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &APFECharacter::JumpEnd);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &APFECharacter::Dash);
	}
}

void APFECharacter::Move(const FInputActionValue& Value)
{
	RawMoveInput = Value.Get<float>();

	if (bIsAlive && bCanMove && (!bIsDoingWallJump || MovementComponent->IsFalling()))
	{
		MoveValue = bBlockHorizontalInput ? WallNormal.X : RawMoveInput;

		if (!bIsGrabbingWall && bIsNearWall)
		{
			if (!bBlockHorizontalInput && WallNormal.X != MoveValue)
			{
				WallGrabStart();
			}
		}
		else
		{
			if (bIsGrabbingWall && bIsNearWall && WallNormal.X == MoveValue)
			{
				WallGrabEnd();
			}
			FlipCharacter(MoveValue);
			AddMovementInput(DirectionRight, MoveValue);
		}
	}
}

void APFECharacter::MoveEnd(const FInputActionValue& Value)
{
	if (bIsGrabbingWall || bIsNearWall)
	{
		WallGrabEnd();
	}
}


void APFECharacter::JumpStart(const FInputActionValue& Value)
{
	if (bIsAlive && bCanMove)
	{
		if (bIsDashing) return;
		
		if (bIsGrabbingWall)
		{
			WallGrabEnd();
			WallJump();
			return;
		}
		
		if (JumpCount < CurrentMetrix.JumpMaxCount)
		{
			bIsJumping = true;
			JumpDelegate.Broadcast();
			MovementComponent->SetMovementMode(MOVE_Falling);
			LaunchCharacter(DirectionUp * MovementComponent->JumpZVelocity, false, true);
			JumpCount++;
			
			if (JumpCount == 1)
			{
				SoundComponent->PlaySound(ESoundType::Jump);
			}
			else if (JumpCount == 2)
			{
				SoundComponent->PlaySound(ESoundType::DoubleJump);
			}
		}
	}
}

void APFECharacter::JumpEnd(const FInputActionValue& Value)
{
	StopJumping();
}

void APFECharacter::Dash(const FInputActionValue& Value)
{
	if (bIsAlive && bCanMove)
	{
		bCanDash = bCanDash &&
					(bIsOnGround ||
					(!bIsOnGround  && DashCountAir < CurrentMetrix.MaxDashInAir) ||
					!bIsGrabbingWall);
	
		if (bCanDash)
		{
			SoundComponent->PlaySound(ESoundType::Dash);
			
			if (!bIsOnGround  && CurrentMetrix.MaxDashInAir < 1) DashCountAir++;
			bCanDash = false;
			bIsDashing = true;
			DisableGravity();
			StartDashDelegate.Broadcast();
		}
		bIsJumping = false;
	}
}

void APFECharacter::EndDash()
{
	// BP : enable gravity if not grabbing
	
	float SpeedX = FMath::Min(FMath::Abs(MovementComponent->Velocity.X), CurrentMetrix.MoveSpeed);
	MovementComponent->Velocity = FVector(SpeedX*MoveValue, 0.f, MovementComponent->Velocity.Z);
	bIsDashing = false;

	if (bIsOnGround)
	{
		FTimerHandle DashCooldownHandle;
		GetWorldTimerManager().SetTimer(DashCooldownHandle,	this, &APFECharacter::ResetDash,
			CurrentMetrix.DashCooldown, false);
	}
}

void APFECharacter::ResetDash()
{
	bCanDash = true;
	DashCountAir = 0;
}

float APFECharacter::GetDashDuration()
{
	return 1/CurrentMetrix.DashDurationInSec;
}

FVector APFECharacter::GetDashVelocity()
{
	return DirectionRight * MoveValue * CurrentMetrix.DashDistance;
}

void APFECharacter::WallGrabStart()
{
	bIsJumping = false;
	GrabWallDelegate.Broadcast(true);
	bIsGrabbingWall = true;
	DisableGravity();
	MovementComponent->Velocity = FVector::Zero();
}

void APFECharacter::WallGrabEnd()
{
	GrabWallDelegate.Broadcast(false);
	bIsGrabbingWall = false;
	EnableGravity();
}

void APFECharacter::WallJump()
{
	bIsDoingWallJump = true;
	bBlockHorizontalInput = true;

	FVector JumpVelocity = (WallNormal + DirectionUp) * WallJumpForce;
	MovementComponent->Velocity = JumpVelocity;
	MoveValue = WallNormal.X;
	FlipCharacter(MoveValue);
	
	FTimerHandle JumpWallHandle;
	GetWorldTimerManager().SetTimer(JumpWallHandle,	this, &APFECharacter::WallJumpReset,
		0.2f, false);
}

void APFECharacter::WallJumpReset()
{
	MoveValue = WallNormal.X;
	bIsDoingWallJump = false;
	bBlockHorizontalInput = false;
}


void APFECharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if (PrevMovementMode == MOVE_Falling && MovementComponent->MovementMode == MOVE_Walking)
	{
		bIsJumping = false;
		JumpCount = 0;

		if (!bCanDash)
		{
			FTimerHandle DashCooldownHandle;
			GetWorldTimerManager().SetTimer(DashCooldownHandle,	this, &APFECharacter::ResetDash,
				CurrentMetrix.DashCooldown, false);
		}
	}
}

void APFECharacter::SwitchMetrix(const FCharacterMetrix& NewMetrix)
{
	CurrentMetrix = NewMetrix;
	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = CurrentMetrix.MoveSpeed;
		MovementComponent->JumpZVelocity = CurrentMetrix.JumpForce;
	}
	DashSpeed = CurrentMetrix.DashDistance / CurrentMetrix.DashDurationInSec;
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

void APFECharacter::StoreKey()
{
	NumberOfKeyPickedUp++;
	UpdateKeyNumberDelegate.Broadcast(NumberOfKeyPickedUp);
}

void APFECharacter::UseKey()
{
	if (NumberOfKeyPickedUp > 0)
	{
		NumberOfKeyPickedUp--;
		UpdateKeyNumberDelegate.Broadcast(NumberOfKeyPickedUp);
	}
}

void APFECharacter::SetReflexionArea(bool bIsInside, float ZPos)
{
	bIsInReflexionArea = bIsInside;
	ReflexionAreaGround = ZPos;
	
	if (!bIsInReflexionArea)
	{
		ReflexionPlane->SetRelativeLocation(ReflexionPlaneLocation);
		ReflexionPlane->SetHiddenInGame(true);
	}
	else
	{
		ReflexionPlane->SetHiddenInGame(false);
	}
}

void APFECharacter::EnableGravity()
{
	MovementComponent->GravityScale = GravityValue;
}

void APFECharacter::DisableGravity()
{
	MovementComponent->GravityScale = 0.f;
}

void APFECharacter::LaunchRespawn()
{
	bIsAlive = false;
	FTimerHandle RespawnHandle;
	GetWorld()->GetTimerManager().SetTimer(
	RespawnHandle, this, &APFECharacter::Respawn, 2.0f, false);
	
	NumberOfKeyPickedUp = 0;
	UpdateKeyNumberDelegate.Broadcast(NumberOfKeyPickedUp);
}

void APFECharacter::Respawn()
{
	FVector RespawnLocation = PFEGameMode->GetCheckpointPosition();
	SetActorLocation(RespawnLocation);
	InitGame();
}

void APFECharacter::PrintOnScreen(const FString& InText)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, InText);
	}
}

