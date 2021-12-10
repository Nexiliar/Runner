// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types.h"
#include "RunnerCharacter.generated.h"

UCLASS(config = Game)
class ARunnerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

public:
	ARunnerCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* CheckComponent = nullptr;

	/** Editable In BP */

	//Show off debug info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		bool Debug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		ESpeedChangeTypes SpeedChangeTypes = ESpeedChangeTypes::FirstType;
	//Distance the character need to overcome before he get a speedboost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		float DistanceToRiseUpSpeed = 0.0f;
	//Amount of boost your speed get after each iteration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "1200", UIMin = "0", UIMax = "1200"), Category = "RunnerCFG")
		float SpeedRiseValue = 0.0f;
	//Need to adjust how often you get speed rise with "Increase Speed OverTime" Mode On
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		float TimeUntillSpeedUp = 0.0f;
	//Scores count the character need to collect before he get a speedboost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		int32 AmountOfScoresToRiseUpSpeed = 0;

	// Line shift
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineShiftCFG")
		class UAnimMontage* ShiftMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineShiftCFG")
		float ShiftMontagePlaySpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineShiftCFG")
		float ShiftOffsetAnimTimeRate = 0.001f;

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	FTimerHandle TimerHandle_SwitchLine;
	FTimerHandle TimerHandle_SpeedRise;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

protected:
	void StartShiftingLine();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Toggle Inputs Handling
	UFUNCTION(BlueprintCallable)
		void EnableInputsHandling();
	UFUNCTION(BlueprintCallable)
		void DisableInputsHandling();

	//SwitchLaneFunctions
	void SwitchRoadLeft();
	void SwitchRoadRight();
	void OffsetCharacterToLane();
	bool CanSwitchLane(bool SwitchSide);

	//SpeedChangeFunctions
	void ChangeSpeed();
	void OverTime();
	UFUNCTION()
		void OverScores(int32 Scores);
	void OverProgress();

	// EndGame
	UFUNCTION(BlueprintCallable)
		bool KillChar();
	UFUNCTION(BlueprintNativeEvent)
		void CharDead_BP();

protected:

	//Check whether its left or right lane
	bool bShiftLeft = false;
	FVector ShiftDestinationPos = FVector(0.0f, 0.0f, 0.0f);
	float LineOffset = 300.f;
	float TimeToShift = 0.2f;
	float ShiftMontagePlayTime = 1.0f;
	float AxisY_Offset = 0.0f;

	// Ignore Or Read inputs
	bool bKeysHandlingEnabled = false;

	//SwitchLaneVariables
	EMovementLine CurrentLine = EMovementLine::LINE_2;
	EMovementLine DestinationLine = EMovementLine::LINE_2;

	bool isOverScores = false;
	FVector CoordToRiseSpeed;
};
