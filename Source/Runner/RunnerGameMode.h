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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
	TSubclassOf<class AMapPartBase> MapPart = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		float MapSpawnSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
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



