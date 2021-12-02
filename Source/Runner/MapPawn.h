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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		int32 MapMaxTileNum = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		int32 MapStartTileNum = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class AObstacleBase>> Obstacles;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void CreateNewTile(bool bOnlyBasic = false);
	UFUNCTION(BlueprintCallable)
		void DeleteLastTile();

	void SpawnObstacle(bool TwoObjects, AMapPartBase* MapTile);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AddTileToMap(AMapPartBase* Tile);
	std::pair<ETileType, TSubclassOf<AMapPartBase>> GetTileType();

protected:

	TArray<FTileInfo> MapBasicTiles;
	TArray<FTileInfo> MapQTETiles;

	Node* MapHead = nullptr;
	Node* MapTail = nullptr;
	int32 CurrentMapLength = 0;
};
