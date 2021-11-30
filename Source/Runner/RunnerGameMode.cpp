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
	
	//Spawning road when game starts
	for (int i = 0; i < StartMapLength; i++)
	{
		SpawnMapPart();
	}	
}

void ARunnerGameMode::SpawnEvent(AMapPartBase* Tile)
{

}

void ARunnerGameMode::SpawnMapPart()
{	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;	
	int8 TileForSpawn = FMath::RandRange(0, 100);
	if (TileForSpawn <= 10)
	{
		AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(MapParts[1], &SpawnPoint, SpawnParams));
		if(NewPart)
			{
				FVector Location = NewPart->ArrowComp->GetComponentLocation();
				SpawnPoint.SetLocation(Location);
				SpawnEvent(NewPart);
			}
	}
	else
	{
		AMapPartBase* NewPart = Cast<AMapPartBase>(GetWorld()->SpawnActor(MapParts[0], &SpawnPoint, SpawnParams));
		if (NewPart)
		{
			FVector Location = NewPart->ArrowComp->GetComponentLocation();
			SpawnPoint.SetLocation(Location);
			Spawn(NewPart);
		}
	}
	//for (int8 i = 0; i< MapParts.Num()-1; i++)

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

void ARunnerGameMode::Spawn(AMapPartBase* Tile)
{
	int8 SpawnChance = FMath::RandRange(0, 100);

	if (SpawnChance <= PercentSpawnChance)
	{
		int8 Lanestooccupy = FMath::RandRange(0, 100);
		if (Lanestooccupy <= 50)
		{
			SpawnObstacle(false, Tile);
		}
		else
		{
			SpawnObstacle(true, Tile);
		}
	}
	if (SpawnChance <= 90)
		SpawnPickUp(Tile);

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