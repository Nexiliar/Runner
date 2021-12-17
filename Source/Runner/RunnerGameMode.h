// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapPawn.h"
#include "GrinchCharacter.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainSettings")
		TSubclassOf<class AMapPawn> MapClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainSettings")
		TSubclassOf<class AGrinchCharacter> GrinchClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float StartSpeed = 800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float MaxCharSpeed = 1500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float MinCharSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float CharSpeedIncreaseAfterObstacle = 1.05f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float GrinchMaxSpeed = 1500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float GrinchMinSpeed = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float GrinchSpeedMultiply = 1.05f;


	//PickUp Classess to randomly spawn on tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class APickUpBase>> PickUps;

	//SpawnChance for obstacles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "100", UIMin = "0", UIMax = "100"), Category = "MapPartSpawningSettings")
		int32 PercentSpawnChance = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "MapPartSpawning")
		void SpawnMapPart();
	UFUNCTION(BlueprintCallable)
		void SpawnBonus();

	UFUNCTION(BlueprintCallable)
		void ChangeScores(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Points")
		int32 GetCurrentScores();

protected:
	int32 CurrentPoints;
	AMapPawn* GameMap = nullptr;
	AGrinchCharacter* Grinch = nullptr;
};
