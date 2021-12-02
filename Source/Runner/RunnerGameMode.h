// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapPawn.h"
#include "RunnerGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoresChange, int32, Scores);

UCLASS(minimalapi)
class ARunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARunnerGameMode();

	// Events
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Scores")
		FOnScoresChange OnScoresChange;

	/** Editable in BP */

	// Map 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
		TSubclassOf<class AMapPawn> MapClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapSettings")
		float StartCharSpeed = 300.f;

	//PickUp Classess to randomly spawn on tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class APickUpBase>> PickUps;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class AObstacleBase>> Obstacles;

	//SpawnChance for obstacles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"), Category = "MapPartSpawningSettings")
		int32 PercentSpawnChance = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "MapPartSpawning")
		void SpawnMapPart();

	void ChangeScores(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Points")
		int32 GetCurrentScores();

	void SpawnPickUp(AMapPartBase* MapTile);
	void SpawnObstacle(bool TwoObjects, AMapPartBase* MapTile);

protected:
	int32 CurrentPoints;
	AMapPawn* GameMap = nullptr;
};
