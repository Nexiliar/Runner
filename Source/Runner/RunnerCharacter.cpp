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
#include "RunnerGameMode.h"

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

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create arrow comp for obstacle check coord
	CheckComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ObstaclesCheck"));
	CheckComponent->SetupAttachment(RootComponent);


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	
	//Init array of Lane Coords
	LaneY.SetNum(3);
	LaneY[0] = -320.0f;
	LaneY[1] = 0.0f;
	LaneY[2] = 320.0f;
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
	ChangeSpeed();
	if (isOverScores)
	{
		ARunnerGameMode* Gamemode = Cast<ARunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (Gamemode)
		{
			Gamemode->OnScoresChange.AddDynamic(this, &ARunnerCharacter::OverScores);
		}
	}	
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


void ARunnerCharacter::SwitchRoadLeft()
{
	isRight = false;
	if (Lane != 0 && !CanSwitchLane(isRight))
	{
		Lane = Lane - 1;		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchSide, this, &ARunnerCharacter::OffsetCharacterToLane, Timer, true);
	}
}

void ARunnerCharacter::SwitchRoadRight()
{
	isRight = true;
	if (Lane != 2 && !CanSwitchLane(isRight))
	{
			Lane = Lane + 1;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchSide, this, &ARunnerCharacter::OffsetCharacterToLane, Timer, true);	
	}
}

void ARunnerCharacter::OffsetCharacterToLane()
{
	FVector GoTo(GetActorLocation().X, LaneY[Lane], GetActorLocation().Z);
		
	if (GetActorLocation() != GoTo)
	{
		if (isRight)
		{
				AddActorWorldOffset(Offcet);			
		}
		else
		{
				AddActorWorldOffset(Offcet * -1);
		}		
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchSide);
	}
}
//Trace to checkout is there is an obstacle
bool ARunnerCharacter::CanSwitchLane(bool SwitchSide)
{
	bool Hit;
	FVector SpawnLocation = CheckComponent->GetComponentLocation();
	FHitResult HitResult;
	FVector RightVector = CheckComponent->GetRightVector();	
	if (!SwitchSide)
	{
		RightVector = RightVector * (-1.0f);
	}		
	RightVector = RightVector * 500 + SpawnLocation;
	Hit = GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation, RightVector, ECollisionChannel::ECC_Visibility);	
	if (!Hit)
	{
		UE_LOG(LogTemp, Warning, TEXT("NotHit"));
		FVector EndUpLocation(60.0f, 0.0f, 0.0f);
		SpawnLocation -= EndUpLocation;
		Hit = GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation, RightVector, ECollisionChannel::ECC_Visibility);
		DrawDebugLine(GetWorld(), SpawnLocation, RightVector, FColor::Green, false, 50.0f);
	}
	DrawDebugLine(GetWorld(), SpawnLocation, RightVector, FColor::Blue,false,50.0f);
	return Hit;
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
	if (GetCharacterMovement()->MaxWalkSpeed <= MaxPlayerSpeedLimit)
	{
		GetCharacterMovement()->MaxWalkSpeed += SpeedRiseValue;
		if (Debug)
			UE_LOG(LogTemp, Warning, TEXT("ARunnerCharacter::OverTime"));
	}

}
//ChangeSpeed OverScores
void ARunnerCharacter::OverScores(int32 Scores)
{
	int32 TempScores = AmountOfScoresToRiseUpSpeed - Scores;
	if (TempScores <= 0)
	{
		if (GetCharacterMovement()->MaxWalkSpeed <= MaxPlayerSpeedLimit)
		{
			AmountOfScoresToRiseUpSpeed += Scores;
			GetCharacterMovement()->MaxWalkSpeed += SpeedRiseValue;
		}

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
		if (GetCharacterMovement()->MaxWalkSpeed <= MaxPlayerSpeedLimit)
		{
			CoordToRiseSpeed = GetCharacterMovement()->GetLastUpdateLocation();
			GetCharacterMovement()->MaxWalkSpeed += SpeedRiseValue;
			if (Debug)
				UE_LOG(LogTemp, Warning, TEXT("ARunnerCharacter::OverProgress %f"), CoordToRiseSpeed.Size());
		}

	}
}

bool ARunnerCharacter::DeadEvent()
{
	if (GetMesh())
	{
		GetCharacterMovement()->MaxWalkSpeed -= GetCharacterMovement()->MaxWalkSpeed;
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
		
		CharDead_BP();
	}
	return true;
}

void ARunnerCharacter::CharDead_BP_Implementation()
{

}

