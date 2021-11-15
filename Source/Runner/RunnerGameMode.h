// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapPartBase.h"
#include "RunnerGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoresChange, int32, Scores);


UCLASS(minimalapi)
class ARunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARunnerGameMode();

	FTimerHandle TimerHandle_SpawnMapBase;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int32 CurrentPoints;	
	

public:
	//MapTileClass to Spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TSubclassOf<class AMapPartBase> MapPart = nullptr;
	//Spawn tile speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.1", ClampMax = "1", UIMin = "0.1", UIMax = "1"), Category = "MapPartSpawningSettings")
		float MapSpawnSpeed = 0.0f;
	//Basic start map lenght generated with tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "30", UIMin = "0", UIMax = "30"), Category = "MapPartSpawningSettings")
		int32 StartMapLength = 0;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Scores")
		FOnScoresChange OnScoresChange;

	FTransform SpawnPoint;

	UFUNCTION(BlueprintCallable, Category = "MapPartSpawning")
		void SpawnMapPart();
	
	void ChangeScores(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Points")
		int32 GetCurrentScores();
	
};



