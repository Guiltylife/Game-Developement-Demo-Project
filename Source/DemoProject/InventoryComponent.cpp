// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < InventoryNumber; i++) {
		Inventories.Add(TArray<FInventoryItem>());
		Inventories[i].Init(EmptyInventoryItem, InventorySize);
	}
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::AddItem(FInventoryItem InItem)
{
	int type = InItem.ItemInfo.Type;
	if (type > Inventories.Num()) return;
	TArray<FInventoryItem>& Items = Inventories[type - 1];

	if (InItem.ItemInfo.CanStacking)
	{
		for (int i = 0; i < InventorySize; i++)
		{
			if (Items[i].ItemInfo.Id == InItem.ItemInfo.Id)
			{
				if (Items[i].Number + InItem.Number <= Items[i].ItemInfo.MaxStackingNumber)
				{
					Items[i].Number += InItem.Number;
					return;
				}
				else
				{
					InItem.Number -= Items[i].ItemInfo.MaxStackingNumber - Items[i].Number;
					Items[i].Number = Items[i].ItemInfo.MaxStackingNumber;
				}
			}
		}
	}

	for (int i = 0; i < InventorySize; i++)
	{
		if (Items[i].ItemInfo.Id == EmptyInventoryItem.ItemInfo.Id)
		{
			Items[i] = InItem;
			return;
		}
	}
}

void UInventoryComponent::RemoveItem(int type, int Index, int Number)
{
	if (Number <= 0) return;
	if (type > Inventories.Num()) return;
	TArray<FInventoryItem>& Items = Inventories[type - 1];
	if (Items[Index].ItemInfo.Id == EmptyInventoryItem.ItemInfo.Id) return;

	if (Number > Items[Index].Number) Number = Items[Index].Number;
	Items[Index].Number -= Number;
	if (Items[Index].Number == 0) {
		Items[Index] = EmptyInventoryItem;
	}
}

void UInventoryComponent::SwapItem(int type, int Index1, int Index2)
{
	if (type > Inventories.Num()) return;
	TArray<FInventoryItem>& Items = Inventories[type - 1];
	Swap(Items[Index1], Items[Index2]);
}

FInventoryItem UInventoryComponent::GetItem(int type, int Index)
{
	if (type > Inventories.Num()) return FInventoryItem();
	TArray<FInventoryItem>& Items = Inventories[type - 1];
	return Items[Index];
}
