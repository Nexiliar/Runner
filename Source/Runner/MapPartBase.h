// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "ObstacleBase.h"
#include "PickUpBase.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MapPartBase.generated.h"

UCLASS()
class RUNNER_API AMapPartBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapPartBase();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComp = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* FloorMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* ArrowComp = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UBoxComponent* BoxComponent = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* Left = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* Mid = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* Right = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class AObstacleBase>> Obstacles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float LifeTime = 0.0f;

	UFUNCTION()
		virtual	void  CollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Spawn();
	void SpawnPickUp();
	void SpawnObstacle(bool TwoObjects);
	
	FVector SpawnRules();
	TArray<bool> OccupiedLanes;
	
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		TArray<TSubclassOf<class APickUpBase>> PickUps;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MapPartSpawningSettings")
		int32 PercentSpawnChance = 0;

	 
};
