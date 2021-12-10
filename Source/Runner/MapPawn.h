// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Types.h"
#include "MapPawn.generated.h"

struct Node
{
	AMapPartBase* Tile = nullptr;
	struct Node* Next = nullptr;
};

UCLASS()
class RUNNER_API AMapPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMapPawn();

	// number of rendering tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		int32 MapMaxTileRuntimeNum = 5;
	// number of tiles created at runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		int32 MapStartTileNum = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		float TimeToChangeLocation = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class AObstacleBase>> Obstacles;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void CreateNewTile();
	UFUNCTION(BlueprintCallable)
		void DeleteLastTile();

	void SpawnObstacle(bool TwoObjects, AMapPartBase* MapTile);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AddTileToMap(AMapPartBase* Tile);
	std::pair<ETileType, TSubclassOf<AMapPartBase>> GetTileType();
	ELandscapeType GetNextEnvironment();
	bool CheckIfEnvExist(ELandscapeType NewEnv);

protected:

	TMap<ELandscapeType, TArray<FTileInfo>> MapsAllTiles;

	Node* MapHead = nullptr;
	Node* MapTail = nullptr;
	int32 CurrentMapLength = 0;
	int32 TileQTESpawnChance = 0;

	ETileType PrevTileType = ETileType::None;
	ELandscapeType CurrentEnv = ELandscapeType::None;
	bool bShouldChangeLocation = false;
	FTimerHandle TimerHandle_ChangeLoc;
};
