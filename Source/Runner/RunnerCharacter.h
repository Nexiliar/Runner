// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types.h"
#include "RunnerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewEffect, float, SpeedScale, float, EffectTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEffectEnd);

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

	/* Collsion box for line switching */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UBoxComponent* LeftShiftBox = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UBoxComponent* RightShiftBox = nullptr;

public:
	ARunnerCharacter();

	// Events
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Effects")
		FOnNewEffect OnNewEffect;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Effects")
		FOnEffectEnd OnEffectEnd;

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

	// Char Speed Buf/Debuf
	UFUNCTION(BlueprintCallable)
		void SetCharSpeed(float NewSpeed);
	UFUNCTION(BlueprintCallable)
		void ChangeSpeedByFactor(float MulFactor);
	UFUNCTION(BlueprintCallable)
		void ChangeSpeedByBuff(float MulFactor, float EffectTime);
	UFUNCTION(BlueprintCallable)
		float GetCharSpeed() const;
	UFUNCTION(BlueprintCallable)
		float GetCharSpeedMinValue() const;
	UFUNCTION(BlueprintCallable)
		float GetCharSpeedScale() const;

	void DisableCharMovement();

	//SwitchLaneFunctions
	UFUNCTION(BlueprintCallable)
		void SwitchRoadLeft();
	UFUNCTION(BlueprintCallable)
		void SwitchRoadRight();
	void OffsetCharacterToLane();

	void CorrectPosition();

	UFUNCTION()
		void CheckForCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// EndGame
	UFUNCTION(BlueprintCallable)
		bool KillChar();
	UFUNCTION(BlueprintNativeEvent)
		void CharDead_BP();

	// animation play rate changing
	UFUNCTION(BlueprintCallable)
		void ChangeAnimPlayRate();
	UFUNCTION(BlueprintNativeEvent)
		void ChangeAnimPlayRate_BP();

	// speed scale chaneged
	UFUNCTION(BlueprintNativeEvent)
		void ChangeCharSpeedScale_BP();

protected:

	float SpeedStartVal = 500.0f;
	float SpeedBeforeEffect = 100.f;
	float SpeedMinVal = 100.f;
	float SpeedMaxVal = 100.f;
	float SpeedScale = 1.0f;	
	float SpeedMaxScale = 2.5f;

	//Check whether its left or right lane
	bool bShiftLeft = false;
	bool bShifting = false;
	FVector ShiftDestinationPos = FVector(0.0f, 0.0f, 0.0f);
	FVector ShiftStartPos = FVector(0.0f, 0.0f, 0.0f);
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

	bool bSpeedUnderEffect = false;
	float SpeedEffectFacot = 1.0f;
};
