// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerCharacter.h"
#include "DrawDebugHelpers.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "RunnerGameMode.h"
#include "ObstacleBase.h"

//////////////////////////////////////////////////////////////////////////
// ARunnerCharacter

ARunnerCharacter::ARunnerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create arrow comp for obstacle check coord
	CheckComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ObstaclesCheck"));
	CheckComponent->SetupAttachment(RootComponent);

	// Create collision box for line switching
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("SwitchBox"));
	BoxComponent->SetupAttachment(RootComponent);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARunnerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("SwitchRoadRight", IE_Pressed, this, &ARunnerCharacter::SwitchRoadRight);
	PlayerInputComponent->BindAction("SwitchRoadLeft", IE_Pressed, this, &ARunnerCharacter::SwitchRoadLeft);
}

void ARunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MoveForward(1.0f);
}

void ARunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CoordToRiseSpeed = GetActorLocation();
	//ChangeSpeed();
	if (isOverScores)
	{
		ARunnerGameMode* Gamemode = Cast<ARunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (Gamemode)
		{
			Gamemode->OnScoresChange.AddDynamic(this, &ARunnerCharacter::OverScores);
		}
	}

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARunnerCharacter::CheckForCollision);
}

void ARunnerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARunnerCharacter::EnableInputsHandling()
{
	bKeysHandlingEnabled = true;
}

void ARunnerCharacter::DisableInputsHandling()
{
	bKeysHandlingEnabled = false;
}

void ARunnerCharacter::SwitchRoadLeft()
{
	if (CurrentLine != EMovementLine::LINE_1 && bKeysHandlingEnabled)
	{
		bShiftLeft = true;
		DestinationLine = (CurrentLine == EMovementLine::LINE_3) ? EMovementLine::LINE_2 : EMovementLine::LINE_1;
		ShiftStartPos = GetActorLocation();
		ShiftDestinationPos = ShiftStartPos + FVector(0.0f, -LineOffset, 0.0f);
		//UE_LOG(LogTemp, Warning, TEXT("ARunnerCharacter::SwitchRoadLeft:  Location - %s, Destination - %s"), *GetActorLocation().ToString(), *ShiftDestinationPos.ToString());
		StartShiftingLine();
	}
}

void ARunnerCharacter::SwitchRoadRight()
{
	if (CurrentLine != EMovementLine::LINE_3 && bKeysHandlingEnabled)
	{
		bShiftLeft = false;
		DestinationLine = (CurrentLine == EMovementLine::LINE_1) ? EMovementLine::LINE_2 : EMovementLine::LINE_3;
		ShiftStartPos = GetActorLocation();
		ShiftDestinationPos = ShiftStartPos + FVector(0.0f, LineOffset, 0.0f);
		//UE_LOG(LogTemp, Warning, TEXT("ARunnerCharacter::SwitchRoadRight:  Location - %s, Destination - %s"), *GetActorLocation().ToString(), *ShiftDestinationPos.ToString());
		StartShiftingLine();
	}
}

void ARunnerCharacter::StartShiftingLine()
{
	// block input
	DisableInputsHandling();

	bShifting = true;

	// play montage
	if (ShiftMontage)
	{
		ShiftMontagePlayTime = ShiftMontage->GetPlayLength();
		PlayAnimMontage(ShiftMontage, ShiftMontagePlaySpeed);
		TimeToShift = ShiftMontagePlayTime / ShiftMontagePlaySpeed;
	}
	else
		TimeToShift = 0.2f;

	AxisY_Offset = (ShiftOffsetAnimTimeRate * LineOffset) / TimeToShift;
	if (bShiftLeft)
		AxisY_Offset *= -1;

	// start timer
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchLine, this, &ARunnerCharacter::OffsetCharacterToLane, ShiftOffsetAnimTimeRate, true);
}

void ARunnerCharacter::OffsetCharacterToLane()
{
	if (TimeToShift <= 0.0f)
	{
		// ClearTimer
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchLine);

		// Correct Position
		if (ShiftMontage)
			GetCapsuleComponent()->AddWorldOffset(FVector(0.0f, ShiftDestinationPos.Y - GetActorLocation().Y, 0.0f));

		// unblock input
		EnableInputsHandling();

		bShifting = false;
		CurrentLine = DestinationLine;
	}
	else
	{
		// move
		FVector Offset(0.0f, AxisY_Offset, 0.0f);
		AddActorWorldOffset(Offset);

		TimeToShift -= ShiftOffsetAnimTimeRate;
	}
}

//Trace to checkout is there is an obstacle TODO!!!
void ARunnerCharacter::CheckForCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ARunnerCharacter::CheckForCollision - Actor: %s"), *OtherActor->GetName());
	if (bShifting)
	{
		UE_LOG(LogTemp, Warning, TEXT("ARunnerCharacter::CheckForCollision - bShifting: %s"), *OtherActor->GetName());
		if (Cast<AObstacleBase>(OtherActor))
		{
			// ClearTimer
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchLine);
			float CurX = GetActorLocation().X;

			SetActorLocation(FVector(CurX, ShiftStartPos.Y, ShiftStartPos.Z));
		}
		
	}
}

//Simple ChangeSpeedFunc
void ARunnerCharacter::ChangeSpeed()
{
	switch (SpeedChangeTypes)
	{
	case ESpeedChangeTypes::FirstType:
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpeedRise, this, &ARunnerCharacter::OverTime, TimeUntillSpeedUp, true);
		break;
	case ESpeedChangeTypes::SecondType:
		isOverScores = true;
		break;
	case ESpeedChangeTypes::ThirdType:
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpeedRise, this, &ARunnerCharacter::OverProgress, 0.5f, true);
		break;
	default:
		break;
	}
}

//ChangeSpeed OverTime
void ARunnerCharacter::OverTime()
{
	GetCharacterMovement()->MaxWalkSpeed += SpeedRiseValue;
	if (Debug)
		UE_LOG(LogTemp, Warning, TEXT("ARunnerCharacter::OverTime"));
}

//ChangeSpeed OverScores
void ARunnerCharacter::OverScores(int32 Scores)
{
	int32 TempScores = AmountOfScoresToRiseUpSpeed - Scores;
	if (TempScores <= 0)
	{
		AmountOfScoresToRiseUpSpeed += Scores;
		GetCharacterMovement()->MaxWalkSpeed += SpeedRiseValue;
	}
	if (Debug)
		UE_LOG(LogTemp, Warning, TEXT("CurrentAmountOfScores = %d"), AmountOfScoresToRiseUpSpeed);
}

//ChangeSpeed OverMapProgress
void ARunnerCharacter::OverProgress()
{
	FVector VectorDifference = GetCharacterMovement()->GetLastUpdateLocation() - CoordToRiseSpeed;
	if (VectorDifference.Size() >= DistanceToRiseUpSpeed)
	{
		CoordToRiseSpeed = GetCharacterMovement()->GetLastUpdateLocation();
		GetCharacterMovement()->MaxWalkSpeed += SpeedRiseValue;
		if (Debug)
			UE_LOG(LogTemp, Warning, TEXT("ARunnerCharacter::OverProgress %f"), CoordToRiseSpeed.Size());
	}
}

bool ARunnerCharacter::KillChar()
{
	if (GetMesh())
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);

		CharDead_BP();
	}
	return true;
}

void ARunnerCharacter::CharDead_BP_Implementation()
{
}