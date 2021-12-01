// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunnerCharacter.generated.h"

UENUM(BlueprintType)
enum class ESpeedChangeTypes : uint8
{
	FirstType UMETA(DisplayName = "Increase Speed OverTime"),
	SecondType UMETA(DisplayName = "Increase Speed Over Scores"),
	ThirdType UMETA(DisplayName = "Increase Speed Over Certain Map Progress")
};

UENUM(BlueprintType)
enum class EMovementLine : uint8
{
	LINE_1,
	LINE_2,
	LINE_3
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		ESpeedChangeTypes SpeedChangeTypes = ESpeedChangeTypes::FirstType;

	/** Line shift */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineShiftCFG")
		class UAnimMontage* ShiftMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineShiftCFG")
		float ShiftMontagePlaySpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LineShiftCFG")
		float ShiftOffsetTimeRate = 0.01f;
	//Check whether its left or right lane
	bool bShiftLeft = false;
	//CharacterOffcetStrengthWhileSwitchingLanes
	FVector ShiftDestinationPos = FVector(0.0f, 0.0f, 0.0f);
	float LineOffset = 300.f;
	bool bShifting = false;
	float TimeToShift = 1.0f;
	float ShiftMontagePlayTime = 1.0f;

	//SwitchLaneVariables
	EMovementLine CurrentLine = EMovementLine::LINE_2;
	EMovementLine DestinationLine = EMovementLine::LINE_2;

	//Amount of boost your speed get after each iteration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "1200", UIMin = "0", UIMax = "1200"), Category = "RunnerCFG")
		float SpeedRiseValue = 0.0f;

	//Need to adjust how often you get speed rise with "Increase Speed OverTime" Mode On
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		float TimeUntillSpeedUp = 0.0f;

	bool isOverScores = false;

	//Distance the character need to overcome before he get a speedboost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		float DistanceToRiseUpSpeed = 0.0f;
	FVector CoordToRiseSpeed;

	//Scores count the character need to collect before he get a speedboost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		int32 AmountOfScoresToRiseUpSpeed = 0;
	//Show off debug info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		bool Debug = false;

	bool DeadEvent();

	UFUNCTION(BlueprintNativeEvent)
		void CharDead_BP();
};
