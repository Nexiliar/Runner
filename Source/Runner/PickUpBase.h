// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpBase.generated.h"

UENUM(BlueprintType)
enum class EDropItem : uint8
{
	COIN,
	BUFF,
	DEBUFF
};

UCLASS()
class RUNNER_API APickUpBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickUpBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComp = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* PickUpMesh = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	EDropItem GetType() const;

public:
	//PickUp object Name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickUpSettings")
		FString ObjectName;
	//Sound to play when pick up
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickUpSettings")
		USoundBase* PickUpSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickUpSettings")
		EDropItem PickUpType = EDropItem::COIN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "20000", UIMin = "0", UIMax = "20000"), Category = "PickUpSettings")
		int32 AmountOfScoresGainOnPickUp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickUpSettings")
		float SpeedMultiply = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PickUpSettings")
		float EffectTime = 5.0f;
};
