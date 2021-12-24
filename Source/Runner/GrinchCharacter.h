// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types.h"
#include "GrinchCharacter.generated.h"

UCLASS()
class RUNNER_API AGrinchCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TSubclassOf<class APickUpBase> DropCoinClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TSubclassOf<class APickUpBase> DropBufsClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		TSubclassOf<class APickUpBase> DropDebufsClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float CoinDistanceDrop = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		bool bRunning = true;

public:
	// Sets default values for this character's properties
	AGrinchCharacter();

	UFUNCTION(BlueprintCallable)
		void DropItem();

	void DisableCharMovement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards movement */
	void MoveForward();

	void TryChangeLane();
	//SwitchLaneFunctions
	void SwitchRoadLeft();
	void SwitchRoadRight();
	void StartShiftingLine();
	void OffsetCharacterToLane();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Char Speed Buf/Debuf
	UFUNCTION(BlueprintCallable)
		void SetCharSpeed(float NewSpeed);
	UFUNCTION(BlueprintCallable)
		float GetCharSpeed() const;
	UFUNCTION(BlueprintCallable)
		void ChangeSpeedByFactor(float MulFactor);

protected:

	float MaxMoveSpeed = 1500.f;
	float MinMoveSpeed = 300.f;
	float MaxSpeedScale = 3.0f;
	float SpeedScale = 1.0f;
	float SpeedStartVal = 300.f;

	//SwitchLaneVariables
	EMovementLine CurrentLine = EMovementLine::LINE_2;
	EMovementLine DestinationLine = EMovementLine::LINE_2;
	FTimerHandle TimerHandle_SwitchLine;

	//Check whether its left or right lane
	bool bShiftLeft = false;
	FVector ShiftDestinationPos = FVector(0.0f, 0.0f, 0.0f);
	float LineOffset = 300.f;
	bool bShifting = false;
	float AxisY_Offset = 0.0f;
	float TimeToShift = 0.2f;
	float ShiftOffsetAnimTimeRate = 0.001f;

	float TimeToChangeLine = 1.0f;
	float ChangeLineTimer = 0.0f;
};
