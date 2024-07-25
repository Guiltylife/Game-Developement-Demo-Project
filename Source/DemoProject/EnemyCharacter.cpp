// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::AddHealth(int Number)
{
	Health += Number;
	Health = FMath::Min(Health, MaxHealth);
}

void AEnemyCharacter::SubHealth(int Number)
{
	Health -= Number;
	Health = FMath::Max(Health, 0);
	if (Health == 0)
	{
		Destroy();
	}
}

void AEnemyCharacter::SetHealth(int Number)
{
	Health = Number;
	Health = FMath::Min(Health, MaxHealth);
	Health = FMath::Max(Health, 0);
}

int AEnemyCharacter::GetHealth()
{
	return Health;
}

int AEnemyCharacter::GetMaxHealth()
{
	return MaxHealth;
}

FString AEnemyCharacter::GetHealthString()
{
	return FString::Printf(TEXT("%d / %d"), Health, MaxHealth);
}
