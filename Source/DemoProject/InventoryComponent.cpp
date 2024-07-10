// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent(int InSize)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Size = InSize;
	Items.Init(EmptyItem, Size);
	for (int i = 0; i < Size; i++) IdToIndex[0].Add(i);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
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
		return true;
	}
	else return false;
}

void UInventoryComponent::RemoveItem(int Index, int Number)
{
	if (Number <= 0) return;
	if (Items[Index].Id == EmptyItem.Id) return;

	Items[Index].Number -= Number;
	if (Items[Index].Number <= 0) Items[Index] = EmptyItem;
}

void UInventoryComponent::SwapItem(int Index1, int Index2)
{
	Swap(Items[Index1], Items[Index2]);
}

void UInventoryComponent::AddCurrentItem(APickUpBase* InItem)
{
	CurrentItems.Add(InItem);
}

void UInventoryComponent::RemoveCurrentItem(APickUpBase* InItem)
{
	CurrentItems.Remove(InItem);
}

void UInventoryComponent::PickItem()
{
	if (CurrentItems.Num() > 0)
	{
		APickUpBase* Item = CurrentItems.Pop();
		AddItem(Item->Item);
		Item->Destroy();
	}
}
