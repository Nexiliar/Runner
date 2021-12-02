// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunnerGameMode.h"
#include "RunnerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARunnerGameMode::ARunnerGameMode()
{
}

void ARunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Create and spawm map pawn
	FVector NewLocation = FVector(-500, 0, 0);
	FTransform NewTransform(NewLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GameMap = Cast<AMapPawn>(GetWorld()->SpawnActor(MapClass, &NewTransform, SpawnParams));
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

void ARunnerGameMode::SpawnPickUp(AMapPartBase* MapTile)
{
	int8 RandPickUpType = FMath::RandRange(0, PickUps.Num() - 1);

	if (PickUps[RandPickUpType].GetDefaultObject()->ObjectName == "Coin")
	{
		int8 Random = FMath::RandRange(1, 9);

		FVector Offcet(400.0f, 0.0f, 0.0f);
		FVector Loc = MapTile->SpawnRules() - Offcet;
		FVector StepBetweenCoins(80.0f, 0.0f, 0.0f);
		for (int8 i = 0; i < Random; i++)
		{
			Loc += StepBetweenCoins;
			APickUpBase* Coin = Cast<APickUpBase>(GetWorld()->SpawnActor(PickUps[RandPickUpType], &Loc));
			if (Coin)
			{
				MapTile->Children.Add(Coin);
			}
		}
	}
}

void ARunnerGameMode::SpawnObstacle(bool TwoObjects, AMapPartBase* MapTile)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;
	FRotator Rot = MapTile->GetActorRotation();

	int8 RandObstacleType = FMath::RandRange(0, Obstacles.Num() - 1);
	FVector SpawnLocation = MapTile->SpawnRules();

	AObstacleBase* Obstacle = Cast<AObstacleBase>(GetWorld()->SpawnActor(Obstacles[RandObstacleType], &SpawnLocation, &Rot, SpawnParams));
	if (Obstacle)
	{
		MapTile->Children.Add(Obstacle);
	}
	if (TwoObjects)
	{
		SpawnObstacle(false, MapTile);
	}
}