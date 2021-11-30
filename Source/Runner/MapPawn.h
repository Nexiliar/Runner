// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MapPartBase.h"
#include "MapPawn.generated.h"

struct Node
{
	AMapPartBase* Tile;
	struct Node* Next;
	struct Node* Prev;
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void AddTileToMap(AMapPartBase* Tile);
	UFUNCTION(BlueprintCallable)
		void CreateNewTile();

	void DeleteLastTile();

protected:

	Node* Head = nullptr;
	FTimerHandle DestoyTileTimer;
};
