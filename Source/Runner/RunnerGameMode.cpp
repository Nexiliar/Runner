// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerGameMode.h"

#include "UObject/ConstructorHelpers.h"
#include "GrinchCharacter.h"
#include "MapPawn.h"

ARunnerGameMode::ARunnerGameMode()
{
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
	FVector GrinchLocation = FVector(500, 0, 480);
	FTransform GrinchTransform(GrinchLocation);
	FActorSpawnParameters GrinchSpawnParams;
	GrinchSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGrinchCharacter* Grinch = Cast<AGrinchCharacter>(GetWorld()->SpawnActor(GrinchClass, &GrinchTransform, GrinchSpawnParams));
}

void ARunnerGameMode::SpawnMapPart()
{
	GameMap->CreateNewTile();
}

void ARunnerGameMode::ChangeScores(int32 Amount)
{
	CurrentPoints += Amount;
	OnScoresChange.Broadcast(CurrentPoints);
}

int32 ARunnerGameMode::GetCurrentScores()
{
	return CurrentPoints;
}
