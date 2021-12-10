// Fill out your copyright notice in the Description page of Project Settings.

#include "GrinchCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PickUpBase.h"

// Sets default values
AGrinchCharacter::AGrinchCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->bRunPhysicsWithNoController = true;
}

void AGrinchCharacter::DropItem()
{
	FVector Location = GetActorLocation() - FVector(100.f, 0.0f, 0.0f);
	FTransform Transform(Location);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APickUpBase* DropItem = Cast<APickUpBase>(GetWorld()->SpawnActor(DropItemClass, &Transform, SpawnParams));
	if (DropItem)
		DropItem->SetLifeSpan(5.0f);
}

// Called when the game starts or when spawned
void AGrinchCharacter::BeginPlay()
{
	Super::BeginPlay();
}

static float DistToDrop = 0.0f;

// Called every frame
void AGrinchCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveForward();

	if (ChangeLineTimer >= TimeToChangeLine)
	{
		TryChangeLane();
		ChangeLineTimer = 0.0f;
	}
	else
		ChangeLineTimer += DeltaTime;
	
	if (DistToDrop >= CoinDistanceDrop && !bShifting)
	{
		DistToDrop = 0.0f;
		DropItem();
	}
	else
		DistToDrop += GetCharacterMovement()->MaxWalkSpeed * DeltaTime;
}

void AGrinchCharacter::MoveForward()
{
	// find out which way is forward
	AddMovementInput(GetActorForwardVector(), 1.0f);
}

void AGrinchCharacter::TryChangeLane()
{
	if (FMath::RandRange(0, 1))
		SwitchRoadRight();
	else
		SwitchRoadLeft();
}

void AGrinchCharacter::SwitchRoadLeft()
{
	if (CurrentLine != EMovementLine::LINE_1 && !bShifting)
	{
		bShiftLeft = true;
		CurrentLine = (CurrentLine == EMovementLine::LINE_3) ? EMovementLine::LINE_2 : EMovementLine::LINE_1;
		ShiftDestinationPos = GetActorLocation() + FVector(0.0f, -LineOffset, 0.0f);
		//UE_LOG(LogTemp, Warning, TEXT("AGrinchCharacter::SwitchRoadLeft():  Location - %s, Destination - %s"), *GetActorLocation().ToString(), *ShiftDestinationPos.ToString());
		StartShiftingLine();
	}
}

void AGrinchCharacter::SwitchRoadRight()
{
	if (CurrentLine != EMovementLine::LINE_3 && !bShifting)
	{
		bShiftLeft = false;
		CurrentLine = (CurrentLine == EMovementLine::LINE_1) ? EMovementLine::LINE_2 : EMovementLine::LINE_3;
		ShiftDestinationPos = GetActorLocation() + FVector(0.0f, LineOffset, 0.0f);
		//UE_LOG(LogTemp, Warning, TEXT("AGrinchCharacter::SwitchRoadRight():  Location - %s, Destination - %s"), *GetActorLocation().ToString(), *ShiftDestinationPos.ToString());
		StartShiftingLine();
	}
}

void AGrinchCharacter::StartShiftingLine()
{
	// block input
	bShifting = true;

	TimeToShift = 0.2f;

	AxisY_Offset = (ShiftOffsetAnimTimeRate * LineOffset) / TimeToShift;
	if (bShiftLeft)
		AxisY_Offset *= -1;

	// start timer
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwitchLine, this, &AGrinchCharacter::OffsetCharacterToLane, ShiftOffsetAnimTimeRate, true);
}

void AGrinchCharacter::OffsetCharacterToLane()
{
	if (TimeToShift <= 0.0f)
	{
		// ClearTimer
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SwitchLine);

		GetCapsuleComponent()->AddWorldOffset(FVector(0.0f, ShiftDestinationPos.Y - GetActorLocation().Y, 0.0f));

		// unblock input
		bShifting = false;
	}
	else
	{
		// move
		FVector Offset(0.0f, AxisY_Offset, 0.0f);
		AddActorWorldOffset(Offset);

		TimeToShift -= ShiftOffsetAnimTimeRate;
	}
}