// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PFECharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
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
	if (FlameComponent)
	{
		FlameComponent->InitFlame();
	}
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
		float Distance;
		FVector NewLocation;
		if (!bIsReflexionHorizontal)
		{
			Distance = FMath::Abs(ReflexionAreaGround - GetActorLocation().X);
			if (MoveValue < 0.f)
			{
				NewLocation = FVector(-(Distance+Distance*DistanceMultInReflexion), ReflexionVPlaneLocation.Y, ReflexionVPlaneLocation.Z);	
			}
			else
			{
				NewLocation = FVector((Distance+Distance*DistanceMultInReflexion), ReflexionVPlaneLocation.Y, ReflexionVPlaneLocation.Z);
			}
			ReflexionPlaneVertical->SetRelativeLocation(NewLocation);
		}
		else
		{
			Distance = FMath::Abs(ReflexionAreaGround - GetActorLocation().Z);
			NewLocation = FVector(
				ReflexionHPlaneLocation.X,
				ReflexionHPlaneLocation.Y,
				-(Distance+Distance*DistanceMultInReflexion)
				);
			ReflexionPlaneHoriontal->SetRelativeLocation(NewLocation);
		}
	}

	if (bIsOnGround)
	{
		LastTimeOnGround = GetWorld()->GetTimeSeconds();
	}
}

void APFECharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this && !OtherActor->Tags.Contains("Platform"))
	{
		bIsNearWall = true;
		CheckWall();
	}
}

void APFECharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bIsNearWall)
	{
		bIsNearWall = false;
		// if (bIsGrabbingWall)
		// {
		// 	WallGrabEnd();
		// }
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
	bIsJumping = false;
	bIsNearWall = false;
	bIsGrabbingWall = false;
	bIsLookingRight = true;

	PFEMovementComponent->ResetVariables();
	//SceneCaptureHorizontal->SetActive(false);
	//SceneCaptureVertical->SetActive(false);
}

void APFECharacter::InitGameMode()
{
	PFEGameMode = Cast<APFEGameMode>(UGameplayStatics::GetGameMode(this));
	check(PFEGameMode);
	PFEGameMode->OnDeath.AddDynamic(this, &APFECharacter::LaunchRespawn);
}

void APFECharacter::FlipCharacter(float Direction)
{
	if (bIsGrabbingWall) return;
	
	if (Direction < 0.0)
	{
		SetActorRotation(LeftOrientation);
		GetController()->SetControlRotation(LeftOrientation);
		bIsLookingRight = false;
	}
	else
	{
		SetActorRotation(RightOrientation);
		GetController()->SetControlRotation(RightOrientation);
		bIsLookingRight = true;
	}
}

void APFECharacter::InitMovementComponent(UCharacterMovementComponent* InMovementComponent)
{
	PFEMovementComponent = Cast<UPFECharacterMovementComponent>(InMovementComponent);
	check(PFEMovementComponent);
}


void APFECharacter::InitCapsuleComponent(UCapsuleComponent* InCapsuleComponent)
{
	CapsuleComponent = InCapsuleComponent;
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &APFECharacter::OnOverlapBegin);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &APFECharacter::OnOverlapEnd);
}

void APFECharacter::InitReflexionPlaneHorizontal(UStaticMeshComponent* InReflexionPlaneHorizontal)
{
	ReflexionPlaneHoriontal = InReflexionPlaneHorizontal;
	ReflexionHPlaneLocation = ReflexionPlaneHoriontal->GetRelativeLocation();
	ReflexionPlaneHoriontal->SetHiddenInGame(true);
}

void APFECharacter::InitReflexionPlaneVertical(UStaticMeshComponent* InReflexionPlaneVertical)
{
	ReflexionPlaneVertical = InReflexionPlaneVertical;
	ReflexionVPlaneLocation = ReflexionPlaneVertical->GetRelativeLocation();
	ReflexionPlaneVertical->SetHiddenInGame(true);
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
	
	if (bIsAlive && bCanMove && (!bIsDoingWallJump || PFEMovementComponent->IsFalling()))
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
	if (bIsGrabbingWall)
	{
		FTimerHandle DetachHandle;
		GetWorld()->GetTimerManager().SetTimer(DetachHandle, this, &APFECharacter::WallGrabEnd, PFEMovementComponent->GetWallDetachDelay(), false);
	}
}


void APFECharacter::JumpStart(const FInputActionValue& Value)
{
	if (!bIsAlive || !bCanMove) return;
	
	bool bIsWallCoyoteTimeValid = (GetWorld()->GetTimeSeconds() - LastWallContactTime) <= PFEMovementComponent->GetWallCoyoteTime();

	if (!bIsOnGround && (bIsGrabbingWall || bIsWallCoyoteTimeValid))
	{
		WallGrabEnd();
		WallJump();
		return;
	}

	LastWallJumpInputTime = GetWorld()->GetTimeSeconds();
	LastJumpInputTime = GetWorld()->GetTimeSeconds();
	
	const float TimeSinceGrounded = GetWorld()->GetTimeSeconds() - LastTimeOnGround;

	const bool bCanUseCoyoteTime = TimeSinceGrounded <= PFEMovementComponent->GetCoyoteTime();

	if (!bIsOnGround || bCanUseCoyoteTime)
	{
		if (JumpCount < 2) JumpDelegate.Broadcast();

		PFEMovementComponent->StartJump();
		
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

void APFECharacter::Dash(const FInputActionValue& Value)
{
	if (bLockDash) return;
	if (!bIsAlive || !bCanMove) return;
	
	if (bCanDash)
	{
		PFEMovementComponent->StartDash(FVector(RawMoveInput, 0,0));
		SoundComponent->PlaySound(ESoundType::Dash);
	}
}

void APFECharacter::WallGrabStart()
{
	if (bIsOnGround) return;

	float PlayerPosX = GetActorLocation().X;
	float DistanceWallPLayer = FMath::Abs(PlayerPosX - WallPosX);
	float OffSet = DistanceWallPLayer - 50.f;
	//OffSet = WallNormal.X < 0 ? -OffSet : OffSet;

	WallGrabDelegate.Broadcast(true, DistanceWallPLayer);
	UE_LOG(LogTemp, Warning, TEXT("DistanceWallPLayer %f"), DistanceWallPLayer);
	
	JumpCount = 0;
	bIsJumping = false;
	bIsGrabbingWall = true;
	PFEMovementComponent->StartWallGrab();
}

void APFECharacter::WallGrabEnd()
{
	bIsGrabbingWall = false;
	WallGrabDelegate.Broadcast(false, 0.f);
	PFEMovementComponent->StopWallGrab();
}

void APFECharacter::WallJump()
{
	if (!bHasStartWallJump) bHasStartWallJump = true;

	JumpDelegate.Broadcast();
	
	SoundComponent->PlaySound(ESoundType::Jump);
	bIsDoingWallJump = true;
	LockInput();

	PFEMovementComponent->StartWallJump(WallNormal.X);
	
	MoveValue = WallNormal.X;
	FlipCharacter(MoveValue);
}

void APFECharacter::LockInput()
{
	bBlockHorizontalInput = true;
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
	
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity,
		ECC_Visibility, FCollisionShape::MakeSphere(PFEMovementComponent->GetWallSphereRadius()), Params);
		
	if (bHit)
	{
		float DotValue = FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector);
			
		if (FMath::Abs(DotValue) < DotThreshold) // is a Wall
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

				WallPosX = HitResult.ImpactPoint.X;
				WallNormal = HitResult.ImpactNormal;
				LastWallContactTime = GetWorld()->GetTimeSeconds();
				return true;
			}
		}
	}
	return false;
}

void APFECharacter::NotifyGround()
{
	bIsOnGround = true;
	bIsJumping = false;
	bHasStartWallJump = false;
	JumpCount = 0;

	PFEMovementComponent->StartTimerResetDash();

	const bool bCanUseBuffer = (GetWorld()->GetTimeSeconds() - LastJumpInputTime) <= PFEMovementComponent->GetJumpBuffer();

	if (bCanUseBuffer)
	{
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("APFECharacter::NotifyGround : active jump buffer "));
#endif
		PFEMovementComponent->StartJump();
		JumpDelegate.Broadcast();
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

void APFECharacter::SetReflexionArea(bool bIsInside, bool bAxisIsHorizontal, const FVector& AxisLocation, float DistanceMult)
{
	bIsInReflexionArea = bIsInside;
	ReflexionAreaGround = bAxisIsHorizontal ? AxisLocation.Z : AxisLocation.X;
	bIsReflexionHorizontal = bAxisIsHorizontal;
	DistanceMultInReflexion = DistanceMult;

	if (!bIsInReflexionArea)
	{
		if (bAxisIsHorizontal)
		{
			ReflexionPlaneHoriontal->SetRelativeLocation(ReflexionHPlaneLocation);
			ReflexionPlaneHoriontal->SetHiddenInGame(true);
			//SceneCaptureHorizontal->SetActive(false);
		}
		else
		{
			ReflexionPlaneVertical->SetRelativeLocation(ReflexionVPlaneLocation);
			ReflexionPlaneVertical->SetHiddenInGame(true);
			//SceneCaptureVertical->SetActive(false);
		}
	}
	else
	{
		if (bAxisIsHorizontal)
		{
			ReflexionPlaneHoriontal->SetHiddenInGame(false);
			//SceneCaptureHorizontal->SetActive(true);
		}
		else
		{
			ReflexionPlaneVertical->SetHiddenInGame(false);
			//SceneCaptureVertical->SetActive(true);
		}
	}
}

void APFECharacter::LaunchRespawn()
{
	bIsAlive = false;
	FTimerHandle RespawnHandle;
	GetWorld()->GetTimerManager().SetTimer(
	RespawnHandle, this, &APFECharacter::Respawn, 2.0f, false);
	
	//NumberOfKeyPickedUp = 0;
	UpdateKeyNumberDelegate.Broadcast(NumberOfKeyPickedUp);
}

void APFECharacter::Respawn()
{
	FVector RespawnLocation = PFEGameMode->GetCheckpointPosition();
	FlameComponent->SetFlameStatus(PFEGameMode->GetCheckpointFlameStatus());
	SetActorLocation(RespawnLocation);
	InitGame();
	if (FlameComponent)
	{
		FlameComponent->ResetFlameAfterDeath();
	}
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

void APFECharacter::ToggleDebugJumpMovement()
{
	PFEMovementComponent->ToggleDebugJumpMovement();
}

void APFECharacter::ToggleDebugWallMovement()
{
	PFEMovementComponent->ToggleDebugWallMovement();
}

void APFECharacter::ToggleDebugFloorCheck()
{
	PFEMovementComponent->ToggleDebugFloorCheck();
}

void APFECharacter::ToggleDebugStateMovement()
{
	PFEMovementComponent->ToggleDebugStateMovement();
}
