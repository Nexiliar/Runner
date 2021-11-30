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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapMoved, FVector, Offset);

UCLASS()
class RUNNER_API AMapPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMapPawn();

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		FOnMapMoved OnMapMoved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawmTile")
		TArray<TSubclassOf<AMapPartBase>> MapElementsTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		bool MapIsMovable = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		int32 MapMaxTileNum = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapParams")
		int32 MapStartTileNum = 3;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AddTileToMap(AMapPartBase* Tile);
	void MovementTick(float DeltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

		
	UFUNCTION(BlueprintCallable)
		void CreateNewTile();
	UFUNCTION(BlueprintCallable)
		void DeleteLastTile();

protected:

	Node* Head = nullptr;
	FTimerHandle DestoyTileTimer;
	int32 CurrentMapLength = 0;
};
