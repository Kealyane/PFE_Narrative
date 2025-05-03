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
#include "Player/PFECharacterMovementComponent.h"

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

	PFEMovementComponent = Cast<UPFECharacterMovementComponent>(GetCharacterMovement());
	check(PFEMovementComponent);
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

	if (bIsOnGround)
	{
		LastTimeOnGround = GetWorld()->GetTimeSeconds();
	}
}

void APFECharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		bIsNearWall = true;
		CheckWall();
// 		FVector Start = GetActorLocation();
// 		FVector End = Start + GetActorForwardVector() * PFEMovementComponent->GetWallDistance();
//
// 		FHitResult HitResult;
// 		FCollisionQueryParams Params;
// 		Params.AddIgnoredActor(this);
//
// 		//bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
// 		bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity,
// 			ECC_Visibility, FCollisionShape::MakeSphere(PFEMovementComponent->GetWallSphereRadius()), Params);
// 		
// 		if (bHit)
// 		{
// 			float DotValue = FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector);
// 			
// 			if (FMath::Abs(DotValue) < DotThreashold) // is a Wall
// 			{
// 				if (HitResult.ImpactNormal.X != MoveValue)
// 				{
// #if WITH_EDITOR
// 					if (PFEMovementComponent->bDebugWallMovement)
// 					{
// 						DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, PFEMovementComponent->GetWallSphereRadius(), 12.f, FColor::Green, false, 2.0f);
// 					}
// #endif
// 					bIsNearWall = true;
// 					WallNormal = HitResult.ImpactNormal;
// 					LastWallContactTime = GetWorld()->GetTimeSeconds();
// 				}
// 			}
// 		}

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

	// if (MovementComponent)
	// {
	// 	SwitchMetrix(SmallFlamesMetrix);
	// }
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
	//SwitchMetrix(SmallFlamesMetrix);
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
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &APFECharacter::Dash);
	}
}

void APFECharacter::Move(const FInputActionValue& Value)
{
	RawMoveInput = Value.Get<float>();
	
	if (bIsAlive && bCanMove && (!bIsDoingWallJump || MovementComponent->IsFalling()))
	{
		MoveValue = bBlockHorizontalInput ? WallNormal.X : RawMoveInput;

		// wall not grabbed and near wall
		if (!bIsGrabbingWall && bIsNearWall)
		{
			// input not blocked and wall normal different from input
			if (!bBlockHorizontalInput && CheckWall() && WallNormal.X != MoveValue)
			{
				WallGrabStart();
			}
			else
			{
				FlipCharacter(MoveValue);
				AddMovementInput(DirectionRight, MoveValue);
			}
		}
		else
		{
			if (bIsGrabbingWall && bIsNearWall && WallNormal.X == MoveValue)
			{
				FTimerHandle DetachHandle;
				GetWorld()->GetTimerManager().SetTimer(DetachHandle, this, &APFECharacter::WallGrabEnd, PFEMovementComponent->GetWallDetachDelay(), false);
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
		FTimerHandle DetachHandle;
		GetWorld()->GetTimerManager().SetTimer(DetachHandle, this, &APFECharacter::WallGrabEnd, PFEMovementComponent->GetWallDetachDelay(), false);
	}
}


void APFECharacter::JumpStart(const FInputActionValue& Value)
{
	bool bIsWallCoyoteTimeValid = (GetWorld()->GetTimeSeconds() - LastWallContactTime) <= PFEMovementComponent->GetWallCoyoteTime();

	if (bIsGrabbingWall || bIsWallCoyoteTimeValid)
	{
		WallGrabEnd();
		WallJump();
		return;
	}

	LastWallJumpInputTime = GetWorld()->GetTimeSeconds();
	LastJumpInputTime = GetWorld()->GetTimeSeconds();
	
	const float TimeSinceGrounded = GetWorld()->GetTimeSeconds() - LastTimeOnGround;

	const bool bCanUseCoyoteTime = TimeSinceGrounded <= PFEMovementComponent->GetCoyoteTime();

	if (bIsOnGround || bCanUseCoyoteTime)
	{
		PFEMovementComponent->StartJump();
	}
	
	// if (bIsAlive && bCanMove)
	// {
	// 	if (bIsDashing) return;
	// 	
	// 	if (bIsGrabbingWall)
	// 	{
	// 		WallGrabEnd();
	// 		WallJump();
	// 		return;
	// 	}
	// 	
	// 	if (JumpCount < CurrentMetrix.JumpMaxCount)
	// 	{
	// 		bIsJumping = true;
	// 		JumpDelegate.Broadcast();
	// 		MovementComponent->SetMovementMode(MOVE_Falling);
	// 		LaunchCharacter(DirectionUp * MovementComponent->JumpZVelocity, false, true);
	// 		JumpCount++;
	// 		
	// 		if (JumpCount == 1)
	// 		{
	// 			SoundComponent->PlaySound(ESoundType::Jump);
	// 		}
	// 		else if (JumpCount == 2)
	// 		{
	// 			SoundComponent->PlaySound(ESoundType::DoubleJump);
	// 		}
	// 	}
	// }
}

void APFECharacter::JumpEnd(const FInputActionValue& Value)
{
	StopJumping();
}

void APFECharacter::Dash(const FInputActionValue& Value)
{
	PFEMovementComponent->StartDash(FVector(RawMoveInput, 0,0));
	// if (bIsAlive && bCanMove)
	// {
	// 	bCanDash = bCanDash &&
	// 				(bIsOnGround ||
	// 				(!bIsOnGround  && DashCountAir < CurrentMetrix.MaxDashInAir) ||
	// 				!bIsGrabbingWall);
	//
	// 	if (bCanDash)
	// 	{
	// 		SoundComponent->PlaySound(ESoundType::Dash);
	// 		
	// 		if (!bIsOnGround  && CurrentMetrix.MaxDashInAir < 1) DashCountAir++;
	// 		bCanDash = false;
	// 		bIsDashing = true;
	// 		DisableGravity();
	// 		StartDashDelegate.Broadcast();
	// 	}
	// 	bIsJumping = false;
	// }
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
	PFEMovementComponent->StartWallGrab();
	// DisableGravity();
	// PFEMovementComponent->Velocity = FVector::Zero();
}

void APFECharacter::WallGrabEnd()
{
	GrabWallDelegate.Broadcast(false);
	bIsGrabbingWall = false;
	PFEMovementComponent->StopWallGrab();
	// EnableGravity();
}

void APFECharacter::WallJump()
{
	if (!bHasStartWallJump) bHasStartWallJump = true;
	
	SoundComponent->PlaySound(ESoundType::Jump);
	bIsDoingWallJump = true;
	bBlockHorizontalInput = true;

	PFEMovementComponent->StartWallJump(WallNormal.X);
	
	MoveValue = WallNormal.X;
	FlipCharacter(MoveValue);
	
	FTimerHandle JumpWallHandle;
	GetWorldTimerManager().SetTimer(JumpWallHandle,	this, &APFECharacter::WallJumpReset,
		PFEMovementComponent->GetWallBlockInputDelay(), false);
}

void APFECharacter::WallJumpReset()
{
	MoveValue = WallNormal.X;
	bIsDoingWallJump = false;
	bBlockHorizontalInput = false;
}

bool APFECharacter::CheckWall()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * PFEMovementComponent->GetWallDistance();

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	//bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(PFEMovementComponent->GetWallSphereRadius()), Params);
		
	if (bHit)
	{
		float DotValue = FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector);
			
		if (FMath::Abs(DotValue) < DotThreashold) // is a Wall
		{
			if (HitResult.ImpactNormal.X != MoveValue)
			{
#if WITH_EDITOR
				if (PFEMovementComponent->bDebugWallMovement)
				{
					DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, PFEMovementComponent->GetWallSphereRadius(), 12.f, FColor::Green, false, 2.0f);
				}
#endif
				if (bHasStartWallJump)
				{
					bool bIsWallInputBufferValid = (GetWorld()->GetTimeSeconds() - LastWallJumpInputTime) <= PFEMovementComponent->GetWallJumpBuffer();
					bool bIsWallCoyoteTimeValid = (GetWorld()->GetTimeSeconds() - LastWallContactTime) <= PFEMovementComponent->GetWallCoyoteTime();
					if (bIsWallInputBufferValid && (bIsNearWall || bIsWallCoyoteTimeValid))
					{
						WallGrabEnd();
						WallJump();
						LastWallJumpInputTime = 0.f;
						return true;
					}
				}
				
				//bIsNearWall = true;
				WallNormal = HitResult.ImpactNormal;
				LastWallContactTime = GetWorld()->GetTimeSeconds();
				return true;
			}
		}
	}
	return false;
}

// void APFECharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
// {
// 	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
// 	
// 	if (PrevMovementMode == MOVE_Falling && MovementComponent->MovementMode == MOVE_Walking)
// 	{
// 		bIsJumping = false;
// 		JumpCount = 0;
//
// 		const bool bCanUseBuffer = (GetWorld()->GetTimeSeconds() - LastJumpInputTime) <= PFEMovementComponent->GetJumpBuffer();
//
// 		if (bCanUseBuffer)
// 		{
// 			PrintOnScreen("APFECharacter::OnMovementModeChanged falling to walking -> jump buffer ok");
// 			PFEMovementComponent->StartJump();
// 		}
// 		// if (!bCanDash)
// 		// {
// 		// 	FTimerHandle DashCooldownHandle;
// 		// 	GetWorldTimerManager().SetTimer(DashCooldownHandle,	this, &APFECharacter::ResetDash,
// 		// 		CurrentMetrix.DashCooldown, false);
// 		// }
// 	}
// }

void APFECharacter::NotifyGround()
{
	bIsOnGround = true;
	bIsJumping = false;
	bHasStartWallJump = false;
	JumpCount = 0;

	const bool bCanUseBuffer = (GetWorld()->GetTimeSeconds() - LastJumpInputTime) <= PFEMovementComponent->GetJumpBuffer();

	if (bCanUseBuffer)
	{
		PrintOnScreen("APFECharacter::NotifyGround falling to walking -> jump buffer ok");
		PFEMovementComponent->StartJump();
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

void APFECharacter::ToggleDebugWalkMovement()
{
	PFEMovementComponent->ToggleDebugWalkMovement();
}
