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
	void AddItem(FInventoryItem InItem);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(int Type, int Index, int Number);

	UFUNCTION(BlueprintCallable)
	void SwapItem(int Type, int Index1, int Index2);

	UFUNCTION(BlueprintCallable)
	FInventoryItem GetItem(int Type, int Index);

	UFUNCTION(BlueprintCallable)
	void UseItem(int Type, int Index);

	UFUNCTION(BlueprintCallable)
	bool CanCombine(int CraftFormulaIndex);

	UFUNCTION(BlueprintCallable)
	void CombineItem(int CraftFormulaIndex);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int InventoryNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int InventorySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* ItemInfoTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* ItemEffectTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* CraftFormulaTable;

private:
	FInventoryItem EmptyInventoryItem;

	TArray<TArray<FInventoryItem>> Inventories;

	TArray<FCraftFormula> CraftFormulas;
};
