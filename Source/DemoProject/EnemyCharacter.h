// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class DEMOPROJECT_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere)
	int Health;

	UPROPERTY(EditAnywhere)
	int MaxHealth;

private:
	UFUNCTION(BlueprintCallable)
	void AddHealth(int Number);

	UFUNCTION(BlueprintCallable)
	void SubHealth(int Number);

	UFUNCTION(BlueprintCallable)
	void SetHealth(int Number);

	UFUNCTION(BlueprintCallable)
	int GetHealth();

	UFUNCTION(BlueprintCallable)
	int GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	FString GetHealthString();
};
