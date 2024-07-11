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

	Items.Init(EmptyItem, Size);
	IdToIndex.Add(0, TSet<int>());
	for (int i = 0; i < Size; i++) IdToIndex[0].Add(i);
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::AddItem(FInventoryItem InItem)
{
	if (IdToIndex.Contains(InItem.Id))
	{
		for (int Index : IdToIndex[InItem.Id])
		{
			if (Items[Index].Number < Items[Index].MaxStackingNumber)
			{
				if (Items[Index].Number + InItem.Number <= Items[Index].MaxStackingNumber)
				{
					Items[Index].Number += InItem.Number;
					return true;
				}
				else
				{
					InItem.Number -= Items[Index].MaxStackingNumber - Items[Index].Number;
					Items[Index].Number = Items[Index].MaxStackingNumber;
				}
			}
		}
	}

	if (IdToIndex.Contains(EmptyItem.Id) && IdToIndex[EmptyItem.Id].Num() > 0)
	{
		int Index = *IdToIndex[EmptyItem.Id].begin();
		IdToIndex[EmptyItem.Id].Remove(Index);
		if (IdToIndex[EmptyItem.Id].Num() == 0) IdToIndex.Remove(EmptyItem.Id);

		Items[Index] = InItem;
		if (!IdToIndex.Contains(InItem.Id)) IdToIndex.Add(InItem.Id, TSet<int>());
		IdToIndex[InItem.Id].Add(Index);

		return true;
	}
	else return false;
}

void UInventoryComponent::RemoveItem(int Index, int Number)
{
	if (Number <= 0) return;
	if (Items[Index].Id == EmptyItem.Id) return;

	if (Number > Items[Index].Number) Number = Items[Index].Number;
	Items[Index].Number -= Number;
	if (Items[Index].Number == 0) {
		IdToIndex[Items[Index].Id].Remove(Index);
		if (IdToIndex[Items[Index].Id].Num() == 0) IdToIndex.Remove(Items[Index].Id);
		Items[Index] = EmptyItem;
		if (!IdToIndex.Contains(0)) IdToIndex.Add(0, TSet<int>());
		IdToIndex[0].Add(Index);
	}
}

void UInventoryComponent::SwapItem(int Index1, int Index2)
{
	FInventoryItem Item1 = GetItem(Index1);
	FInventoryItem Item2 = GetItem(Index2);

	IdToIndex[Item1.Id].Remove(Index1);
	IdToIndex[Item2.Id].Remove(Index2);
	IdToIndex[Item1.Id].Add(Index2);
	IdToIndex[Item2.Id].Add(Index1);

	Swap(Items[Index1], Items[Index2]);
}

FInventoryItem UInventoryComponent::GetItem(int Index)
{
	return Items[Index];
}
