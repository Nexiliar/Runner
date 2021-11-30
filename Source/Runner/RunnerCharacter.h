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

UCLASS(config=Game)
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* CheckComponent = nullptr;
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
protected:


	/** Called for forwards/backward input */
	void MoveForward(float Value);

	
	FTimerHandle TimerHandle_SwitchSide;
	FTimerHandle TimerHandle_SpeedRise;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

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
	
	//Basic Y-Coord of lane to switch
		TArray<float> LaneY;
	//CharacterOffcetStrengthWhileSwitchingLanes	
		FVector Offcet = FVector(0.0f, 5.0f, 0.0f);
	//Timer Value to do offcet	
		float Timer = 0.001f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		ESpeedChangeTypes SpeedChangeTypes = ESpeedChangeTypes::FirstType;
	
	//Amount of boost your speed get after each iteration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "1200", UIMin = "0", UIMax = "1200"), Category = "RunnerCFG")
		float SpeedRiseValue = 0.0f;

	//Need to adjust how often you get speed rise with "Increase Speed OverTime" Mode On
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RunnerCFG")
		float TimeUntillSpeedUp = 0.0f;	

	//SwitchLaneVariables
	//Current lane
	int32 Lane = 1;	
	//Check whether its left or right lane
	bool isRight = false;
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
