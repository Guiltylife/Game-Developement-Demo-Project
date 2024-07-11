// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickUpBase.h"
#include "InventoryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class DEMOPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool AddItem(FInventoryItem InItem);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(int Index, int Number);

	UFUNCTION(BlueprintCallable)
	void SwapItem(int Index1, int Index2);

	UFUNCTION(BlueprintCallable)
	FInventoryItem GetItem(int Index);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryItem EmptyItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Size;

private:
	TArray<FInventoryItem> Items;

	TMap<int, TSet<int>> IdToIndex;
};
