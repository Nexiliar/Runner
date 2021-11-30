// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
//#include "MapPartBase.h"
#include "MapPawn.h"
#include "RunnerGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoresChange, int32, Scores);

UCLASS(minimalapi)
class ARunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARunnerGameMode();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 CurrentPoints;

	AMapPawn* mMap;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
		TSubclassOf<class AMapPawn> MapClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
		float MapStartSpeed = 300.f;

	//MapTileClass to Spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class AMapPartBase>> MapParts;

	//Basic start map lenght generated with tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "30", UIMin = "0", UIMax = "30"), Category = "MapPartSpawningSettings")
		int32 StartMapLength = 0;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Scores")
		FOnScoresChange OnScoresChange;

	//PickUp Classess to randomly spawn on tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class APickUpBase>> PickUps;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class AObstacleBase>> Obstacles;
	//SpawnChance for obstacles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"), Category = "MapPartSpawningSettings")
		int32 PercentSpawnChance = 0;

	FTransform SpawnPoint;
	UFUNCTION(BlueprintCallable, Category = "MapPartSpawning")
		void SpawnMapPart();

	void ChangeScores(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Points")
		int32 GetCurrentScores();

	void SpawnEvent(AMapPartBase* Tile);

	void Spawn(AMapPartBase* Tile);
	void SpawnPickUp(AMapPartBase* MapTile);
	void SpawnObstacle(bool TwoObjects, AMapPartBase* MapTile);
};
