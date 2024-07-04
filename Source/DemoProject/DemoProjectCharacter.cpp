// Copyright Epic Games, Inc. All Rights Reserved.

#include "DemoProjectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ADemoProjectCharacter

ADemoProjectCharacter::ADemoProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ADemoProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnClimbEdgeTickCallback.BindUFunction(this, TEXT("ClimbEdgeTickCallback"));
	ClimbEdgeTimeLine.AddInterpFloat(ClimbEdgeCurve, OnClimbEdgeTickCallback);

	OnClimbEdgeFinishCallback.BindUFunction(this, TEXT("ClimbEdgeFinishCallback"));
	ClimbEdgeTimeLine.SetTimelineFinishedFunc(OnClimbEdgeFinishCallback);
}

void ADemoProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ClimbEdgeTimeLine.TickTimeline(DeltaTime);

	TickSwim();

	TickClimb();
}

void ADemoProjectCharacter::TickSwim()
{
	if (bIsInWater)
	{
		if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Swimming && GetActorLocation().Z < WaterHeight + OverwaterOffset)
		{
			GetPawnPhysicsVolume()->bWaterVolume = true;
			GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Swimming;
			bIsRaising = true;
		}
		if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Swimming) {
			if (bIsDiving) {
				MoveToUnderwater();
			}

			if (bIsUnderWater && GetActorLocation().Z + 20 > WaterHeight + OverwaterOffset) {
				bIsRaising = true;
				bRecieveUserInput = false;
			}
			
			if (bIsRaising) {
				MoveToOverwater();
			}

			if (GetActorLocation().Z > WaterHeight + OverwaterOffset + 5) {
				GetPawnPhysicsVolume()->bWaterVolume = false;
				GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
			}
		}
	}
}

void ADemoProjectCharacter::TickClimb()
{
	if (bIsClimbing && !bIsClimbingEdge)
	{
		FHitResult HitResult;
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(),
			GetActorLocation() + GetActorForwardVector() * 90, 30,
			ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::ForOneFrame, HitResult, true))
		{
			if (HitResult.Actor->Tags.Contains(FName("Landscape")))
			{
				SetActorLocation(HitResult.Location + HitResult.Normal * 5);
				SetActorRotation(UKismetMathLibrary::FindLookAtRotation(HitResult.Normal, FVector::ZeroVector));

				float WallDegree = abs(FMath::RadiansToDegrees((FMath::Acos(FVector::DotProduct(FVector::UpVector, HitResult.Normal)))));
				if (WallDegree < 45)
				{
					TickClimbEdge();
				}
				else if (WallDegree > 150)
				{
					Climb();
				}

				if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation() + GetActorUpVector() * 130,
					GetActorLocation() + GetActorUpVector() * 130 + GetActorForwardVector() * 90, 45,
					ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::ForOneFrame, HitResult, true))
				{
					if (!HitResult.Actor->Tags.Contains(FName("Landscape"))) TickClimbEdge();
				}
				else
				{
					TickClimbEdge();
				}

				if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(),
					GetActorLocation() - GetActorUpVector() * 112,
					ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::ForOneFrame, HitResult, true))
				{
					Climb();
				}
			}
			else Climb();
		}
		else Climb();
	}
}

void ADemoProjectCharacter::TickClimbEdge()
{
	bRecieveUserInput = false;
	bIsClimbingEdge = true;

	ClimbEdgeRotatorStart = GetActorRotation();
	ClimbEdgeRotatorEnd = GetActorRotation();
	ClimbEdgeRotatorEnd.Pitch = 0;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ClimbEdgeTimeLine.PlayFromStart();
}

void ADemoProjectCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	SetActorLocation(Level1StartPoint);
}

/** Move character to over water */
void ADemoProjectCharacter::MoveToOverwater()
{
	if (GetActorLocation().Z < WaterHeight + OverwaterOffset)
	{
		FVector Direction = GetActorForwardVector();
		Direction.Z = 0;
		AddMovementInput(FVector::UpVector + Direction * 0.01);
	}
	else {
		bIsUnderWater = false;
		bIsRaising = false;
		bRecieveUserInput = true;
		FRotator NewRotator = GetActorRotation();
		NewRotator.Pitch = 0;
		SetActorRotation(NewRotator);
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

/** Move character to underwater */
void ADemoProjectCharacter::MoveToUnderwater()
{
	if (GetActorLocation().Z > WaterHeight + UnderwaterOffset)
	{
		FVector Direction = GetActorForwardVector();
		Direction.Z = 0;
		AddMovementInput(FVector::DownVector + Direction * 0.01);
	}
	else {
		bIsUnderWater = true;
		bIsDiving = false;
		bRecieveUserInput = true;
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

/** Call when climb edge time line tick */
void ADemoProjectCharacter::ClimbEdgeTickCallback()
{
	float FloatCurveValue = ClimbEdgeCurve->GetFloatValue(ClimbEdgeTimeLine.GetPlaybackPosition());
	SetActorRotation(FMath::Lerp(ClimbEdgeRotatorStart, ClimbEdgeRotatorEnd, FloatCurveValue));
}

/** Call when climb edge time line finish */
void ADemoProjectCharacter::ClimbEdgeFinishCallback()
{
	bRecieveUserInput = true;

	Climb();

	bIsClimbingEdge = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADemoProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADemoProjectCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ADemoProjectCharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADemoProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADemoProjectCharacter::MoveRight);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ADemoProjectCharacter::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ADemoProjectCharacter::Walk);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADemoProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADemoProjectCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ADemoProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ADemoProjectCharacter::TouchStopped);

	PlayerInputComponent->BindAction("Fly", IE_Pressed, this, &ADemoProjectCharacter::Fly);

	PlayerInputComponent->BindAction("Dive", IE_Pressed, this, &ADemoProjectCharacter::Dive);

	PlayerInputComponent->BindAction("Climb", IE_Pressed, this, &ADemoProjectCharacter::Climb);
}

void ADemoProjectCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ADemoProjectCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ADemoProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADemoProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADemoProjectCharacter::MoveForward(float Value)
{
	if (!bRecieveUserInput) return;

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		EMovementMode MovementMode = GetCharacterMovement()->MovementMode;
		if (MovementMode == MOVE_Swimming) // Swimming
		{
			if (bIsUnderWater) // underwater
			{
				// find out which way is forward
				const FRotator Rotation = Controller->GetControlRotation();

				// get forward vector
				const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

				if (!bIsRunning) Value /= 2;
				AddMovementInput(Direction, Value);

				SetActorRotation(FMath::Lerp(GetActorRotation(), Rotation, 0.1f));
			}
			else // over water
			{
				// find out which way is forward
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

				if (!bIsRunning) Value /= 2;
				AddMovementInput(Direction, Value);
			}
		}
		else if (MovementMode == MOVE_Flying && !bIsClimbing) // flying
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();

			// get forward vector
			const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);

			if (!bIsRunning) Value /= 2;
			AddMovementInput(Direction, Value);
		}
		else if (MovementMode == MOVE_Flying && bIsClimbing) // climbing
		{
			AddMovementInput(GetActorUpVector(), Value);
		}
		else
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			if (!bIsRunning) Value /= 2;
			AddMovementInput(Direction, Value);
		}
	}
}

void ADemoProjectCharacter::MoveRight(float Value)
{
	if (!bRecieveUserInput) return;

	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		EMovementMode MovementMode = GetCharacterMovement()->MovementMode;
		if (MovementMode == MOVE_Flying && bIsClimbing) // climbing
		{
			AddMovementInput(GetActorRightVector(), Value);
		}
		else
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			if (!bIsRunning) Value /= 2;
			AddMovementInput(Direction, Value);
		}
	}
}

void ADemoProjectCharacter::Jump()
{
	if (!bRecieveUserInput) return;

	if (GetCharacterMovement()->MovementMode == MOVE_Flying) {
		if (bIsClimbing)
		{
			GetCharacterMovement()->AddImpulse(GetActorUpVector() * 500, true);
		}
		else
		{
			GetCharacterMovement()->AddImpulse(FVector::UpVector * 500, true);
		}
		
	}
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		Fly();
	}
	else {
		Super::Jump();
	}
}

void ADemoProjectCharacter::StopJumping()
{
	if (GetCharacterMovement()->MovementMode == MOVE_Flying) {
		return;
	}
	else {
		Super::StopJumping();
	}
}

void ADemoProjectCharacter::Run()
{
	if (!bRecieveUserInput) return;

	bIsRunning = true;
}

void ADemoProjectCharacter::Walk()
{
	if (!bRecieveUserInput) return;

	bIsRunning = false;
}

void ADemoProjectCharacter::Fly()
{
	if (!bRecieveUserInput) return;

	if (GetCharacterMovement()->MovementMode == MOVE_Flying) {
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else {
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		bIsClimbing = false;

		GetCharacterMovement()->AddImpulse(FVector::UpVector * 50, true);
	}
}

void ADemoProjectCharacter::Dive()
{
	if (!bRecieveUserInput) return;

	if (bIsInWater && !bIsUnderWater) {
		bIsDiving = true;
		bRecieveUserInput = false;
	}
}

void ADemoProjectCharacter::Climb()
{
	if (!bRecieveUserInput) return;

	if (GetCharacterMovement()->MovementMode == MOVE_Walking && !bIsClimbing) {
		FHitResult HitResult;
		if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 90, 30, ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::ForOneFrame, HitResult, true))
		{
			if (HitResult.Actor->ActorHasTag("Climbable")) {
				GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
				bIsClimbing = true;

				GetCharacterMovement()->bOrientRotationToMovement = false;

				GetCharacterMovement()->MaxFlySpeed = 200;
				GetCharacterMovement()->BrakingDecelerationFlying = 1000;

				GetCapsuleComponent()->SetCapsuleHalfHeight(105);
				GetMesh()->SetRelativeLocation(FVector(0, 0, -114));
			}
		}
	}
	else if (GetCharacterMovement()->MovementMode == MOVE_Flying && bIsClimbing)
	{
		GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
		bIsClimbing = false;

		GetCharacterMovement()->bOrientRotationToMovement = true;

		GetCharacterMovement()->MaxFlySpeed = 600;
		GetCharacterMovement()->BrakingDecelerationFlying = 0;

		GetCapsuleComponent()->SetCapsuleHalfHeight(110);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -107));

		FRotator NewRotator = GetActorRotation();
		NewRotator.Pitch = 0;
		SetActorRotation(NewRotator);
	}
}