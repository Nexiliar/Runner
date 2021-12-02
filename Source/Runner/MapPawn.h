// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MapPartBase.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawmTile")
		TArray<TSubclassOf<AMapPartBase>> MapElementsTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		int32 MapMaxTileNum = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		int32 MapStartTileNum = 3;

public:	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		
	UFUNCTION(BlueprintCallable)
		void CreateNewTile();
	UFUNCTION(BlueprintCallable)
		void DeleteLastTile();

protected:
	void AddTileToMap(AMapPartBase* Tile);

protected:

	Node* MapHead = nullptr;
	Node* MapTail = nullptr;
	int32 CurrentMapLength = 0;
};
