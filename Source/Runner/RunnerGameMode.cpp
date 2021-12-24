// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerGameMode.h"
#include "UObject/ConstructorHelpers.h"

ARunnerGameMode::ARunnerGameMode()
{
}

void ARunnerGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float PlayerX = Player->GetActorLocation().X;
	float GrinchX = Grinch->GetActorLocation().X;

	if (((GrinchX - PlayerX) <= 150) && bGameStarted)
	{
		Player->DisableCharMovement();
		Grinch->DisableCharMovement();

		Grinch->SetActorLocation(Player->GetActorLocation() + FVector(200.f, 0.0f, 0.0f));

		GameEnded_BP();
	}

}

void ARunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Create and spawm map pawn
	FVector MapLocation = FVector(-500, 0, 0);
	FTransform MapTransform(MapLocation);
	FActorSpawnParameters MapSpawnParams;
	MapSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GameMap = Cast<AMapPawn>(GetWorld()->SpawnActor(MapClass, &MapTransform, MapSpawnParams));

	// Create and spawm map pawn
	FVector GrinchLocation = FVector(0, 0, 480);
	FTransform GrinchTransform(GrinchLocation);
	FActorSpawnParameters GrinchSpawnParams;
	GrinchSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Grinch = Cast<AGrinchCharacter>(GetWorld()->SpawnActor(GrinchClass, &GrinchTransform, GrinchSpawnParams));
	Grinch->SetCharSpeed(StartSpeed);

	Player = Cast<ARunnerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void ARunnerGameMode::SpawnMapPart()
{
	GameMap->CreateNewTile();
	//Grinch->SetCharSpeed(Grinch->GetCharSpeed() * GrinchSpeedMultiply);
	Grinch->ChangeSpeedByFactor(GrinchSpeedMultiply);
}

void ARunnerGameMode::SpawnBonus()
{
	if (Grinch)
		Grinch->DropItem();
}

void ARunnerGameMode::ChangeScores(int32 Amount)
{
	CurrentPoints += Amount;
	OnScoresChange.Broadcast(CurrentPoints, Amount);
	//UE_LOG(LogTemp, Warning, TEXT("ARunnerGameMode::ChangeScores  CurrentPoints %i"), CurrentPoints);
}

int32 ARunnerGameMode::GetCurrentScores()
{
	return CurrentPoints;
}

void ARunnerGameMode::StartGame()
{
	bGameStarted = true;
}

void ARunnerGameMode::StopGame()
{
	bGameStarted = false;
}

void ARunnerGameMode::GameEnded_BP_Implementation()
{
}
