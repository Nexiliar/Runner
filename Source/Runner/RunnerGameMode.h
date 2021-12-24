// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapPawn.h"
#include "GrinchCharacter.h"
#include "RunnerCharacter.h"
#include "RunnerGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoresChange, int32, CurrScores, int32, ScoreChage);

UCLASS(minimalapi)
class ARunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARunnerGameMode();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
		float MaxCharSpeed = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float MinCharSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float CharSpeedIncreaseAfterObstacle = 1.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float MaxCharSpeedScale = 2.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float GrinchMaxSpeed = 3000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float GrinchMinSpeed = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float GrinchSpeedMultiply = 1.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeedSettings")
		float GrinchSpeedMaxScale = 2.5f;


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

	UFUNCTION(BlueprintCallable)
		void StartGame();
	UFUNCTION(BlueprintCallable)
		void StopGame();

	UFUNCTION(BlueprintNativeEvent)
		void GameEnded_BP();

protected:
	int32 CurrentPoints;
	bool bGameStarted = false;

	ARunnerCharacter* Player = nullptr;
	AMapPawn* GameMap = nullptr;
	AGrinchCharacter* Grinch = nullptr;
};
