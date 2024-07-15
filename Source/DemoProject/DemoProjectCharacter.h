// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TimeLineComponent.h"
#include "DemoProjectCharacter.generated.h"

UCLASS(config=Game)
class ADemoProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ADemoProjectCharacter();

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	void TickSwim();

	void TickClimb();

	void TickClimbEdge();

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseLookUpRate;

	/** Overwater character offset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Swim)
	float OverwaterOffset = -25;

	/** Underwater character offset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Swim)
	float UnderwaterOffset = -100;

	/** Current Character Status. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buff)
	TSet<int> Buffs;

	/** Current Level Number. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Level)
	int LevelNumber;

	/** Level 1 Start Point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Level)
	FVector Level1StartPoint = FVector(-4650, 0, -200);

	/** Level 2 Start Point. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Level)
	FVector Level2StartPoint = FVector(0, 0, 260);

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called for jump */
	void Jump();

	/** Called for stop jump */
	void StopJumping();

	/** Called for run */
	void Run();

	/** Called for walk */
	void Walk();

	/* Called for Fly */
	void Fly();

	/** Called for Dive */
	void Dive();

	/** Called for Climb */
	void Climb();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Move character to over water */
	void MoveToOverwater();

	/** Move character to underwater */
	void MoveToUnderwater();

	/** Call when climb edge time line tick */
	UFUNCTION()
	void ClimbEdgeTickCallback();

	/** Call when climb edge time line finish */
	UFUNCTION()
	void ClimbEdgeFinishCallback();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns Character Movement Mode */
	UFUNCTION(BlueprintCallable)
	EMovementMode GetMovementMode() const
	{
		UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
		if (CharacterMovementComponent) {
			return CharacterMovementComponent->MovementMode;
		}
		return EMovementMode::MOVE_None;
	}

	/** Returns Character Front Direction */
	UFUNCTION(BlueprintCallable)
	FVector GetFrontDirection() const
	{
		if (Controller) {
			return FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis(EAxis::X);
		}
		return FVector::ZeroVector;
	}

	/** Returns CHaracter Front Rotation */
	UFUNCTION(BlueprintCallable)
	FRotator GetFrontRotation() const
	{
		if (Controller) {
			FRotator Rotator =  Controller->GetControlRotation();
			if (Rotator.Roll > 180) Rotator.Roll -= 360;
			if (Rotator.Pitch > 180) Rotator.Pitch -= 360;
			if (Rotator.Yaw > 180) Rotator.Yaw -= 360;
			return Rotator;
		}
		return FRotator::ZeroRotator;
	}

	UFUNCTION(BlueprintCallable)
	void SetIsInWater(bool IsInWater)
	{
		bIsInWater = IsInWater;
	}

	UFUNCTION(BlueprintCallable)
	void SetWaterHeight(float InWaterHeight)
	{
		WaterHeight = InWaterHeight;
	}

	UFUNCTION(BlueprintCallable)
	bool IsUnderWater() const
	{
		return bIsUnderWater;
	}

	UFUNCTION(BlueprintCallable)
	bool IsClimbing() const
	{
		return bIsClimbing;
	}

	UFUNCTION(BlueprintCallable)
	bool IsClimbingEdge() const
	{
		return bIsClimbingEdge;
	}

	UFUNCTION(BlueprintCallable)
	bool IsWalkIK() const
	{
		return GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking && GetVelocity().Size() < 400;
	}
	
	UFUNCTION(BlueprintCallable)
	bool IsClimbIK() const
	{
		/*return bIsClimbing && !bIsClimbingEdge && GetVelocity().Size() < 100;*/
		return bIsClimbing && !bIsClimbingEdge;
	}

private:
	/** True if receive user input */
	bool bRecieveUserInput = true;

	/** True if the character is running */
	bool bIsRunning = false;

	/** True if the character is in water */
	bool bIsInWater = false;

	/** Water height */
	float WaterHeight;

	/** True if the character is underwater */
	bool bIsUnderWater = false;

	/** True if the character is diving */
	bool bIsDiving = false;

	/** True if the character is raising */
	bool bIsRaising = false;

	/** True if the character is climbing */
	bool bIsClimbing = false;

	/** True if the character is climbing edge */
	bool bIsClimbingEdge = false;

private:
	FTimeline ClimbEdgeTimeLine;

	UPROPERTY(EditAnywhere)
	UCurveFloat* ClimbEdgeCurve;

	FOnTimelineFloatStatic OnClimbEdgeTickCallback;

	FOnTimelineEventStatic OnClimbEdgeFinishCallback;

	FRotator ClimbEdgeRotatorStart;

	FRotator ClimbEdgeRotatorEnd;
};

