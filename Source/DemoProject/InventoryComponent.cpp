// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DemoProjectCharacter.h"

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

	for (int i = 0; i < InventoryNumber; i++)
	{
		Inventories.Add(TArray<FInventoryItem>());
		Inventories[i].Init(EmptyInventoryItem, InventorySize);
	}

	for (auto it : CraftFormulaTable->GetRowMap())
	{
		CraftFormulas.Add(*((FCraftFormula*)it.Value));
	}
	while (CraftFormulas.Num() < InventorySize)
	{
		CraftFormulas.Add(FCraftFormula());
	}
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::AddItem(FInventoryItem InItem)
{
	int Type = InItem.ItemInfo.Type;
	if (Type > Inventories.Num()) return;
	TArray<FInventoryItem>& Items = Inventories[Type - 1];

	if (InItem.ItemInfo.CanStacking)
	{
		for (int i = 0; i < InventorySize; i++)
		{
			if (Items[i].ItemInfo.Id == InItem.ItemInfo.Id)
			{
				if (Items[i].ItemNumber + InItem.ItemNumber <= Items[i].ItemInfo.MaxStackingNumber)
				{
					Items[i].ItemNumber += InItem.ItemNumber;
					return;
				}
				else
				{
					InItem.ItemNumber -= Items[i].ItemInfo.MaxStackingNumber - Items[i].ItemNumber;
					Items[i].ItemNumber = Items[i].ItemInfo.MaxStackingNumber;
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

void UInventoryComponent::RemoveItem(int Type, int Index, int Number)
{
	if (Number <= 0) return;
	if (Type > Inventories.Num() || Type <= 0) return;
	TArray<FInventoryItem>& Items = Inventories[Type - 1];
	if (Items[Index].ItemInfo.Id == EmptyInventoryItem.ItemInfo.Id) return;

	if (Number > Items[Index].ItemNumber) Number = Items[Index].ItemNumber;
	Items[Index].ItemNumber -= Number;
	if (Items[Index].ItemNumber == 0) {
		Items[Index] = EmptyInventoryItem;
	}
}

void UInventoryComponent::SwapItem(int Type, int Index1, int Index2)
{
	if (Type > Inventories.Num() || Type <= 0) return;
	TArray<FInventoryItem>& Items = Inventories[Type - 1];
	Swap(Items[Index1], Items[Index2]);
}

FInventoryItem UInventoryComponent::GetItem(int Type, int Index)
{
	if (Type > Inventories.Num() || Type == 0) return FInventoryItem();

	if (Type == -1)
	{
		int ItemId = CraftFormulas[Index].CompositeId;
		if (ItemId == 0) return FInventoryItem();

		FInventoryItem Item = FInventoryItem();
		if (FInventoryItemInfo* ItemInfo = ItemInfoTable->FindRow<FInventoryItemInfo>(FName(FString::FromInt(ItemId)), TEXT("")))
		{
			Item.ItemInfo = *ItemInfo;
			Item.ItemNumber = CraftFormulas[Index].CompositeNumber;
		}
		return Item;
	}
	else
	{
		return Inventories[Type - 1][Index];
	}
	
}

void UInventoryComponent::UseItem(int Type, int Index)
{
	if (Type == 2)
	{
		if (FItemEffect* ItemEffect = ItemEffectTable->FindRow<FItemEffect>(FName(FString::FromInt(Inventories[Type - 1][Index].ItemInfo.Id)), TEXT("")))
		{
			for (int Buff : ItemEffect->CharacterBuffs)
			{
				Cast<ADemoProjectCharacter>(GetOwner())->Buffs.Add(Buff);
			}
		}
	}
}

bool UInventoryComponent::CanCombine(int CraftFormulaIndex)
{
	FCraftFormula CraftFormula = CraftFormulas[CraftFormulaIndex];
	if (FInventoryItemInfo* CompositeInfo = ItemInfoTable->FindRow<FInventoryItemInfo>(FName(FString::FromInt(CraftFormula.CompositeId)), TEXT("")))
	{
		TArray<FInventoryItem>& Inventory = Inventories[CompositeInfo->Type - 1];
		for (int i = 0; i < CraftFormula.IngrediantIds.Num(); i++)
		{
			int IngrediantNumber = 0;
			for (int j = 0; j < Inventory.Num(); j++)
			{
				if (Inventory[j].ItemInfo.Id == CraftFormula.IngrediantIds[i]) IngrediantNumber += Inventory[j].ItemNumber;
				if (IngrediantNumber >= CraftFormula.IngrediantNumbers[i]) break;
			}
			if (IngrediantNumber < CraftFormula.IngrediantNumbers[i]) return false;
		}

		return true;
	}
	else return false;
}

void UInventoryComponent::CombineItem(int CraftFormulaIndex)
{
	if (!CanCombine(CraftFormulaIndex)) return;

	FCraftFormula CraftFormula = CraftFormulas[CraftFormulaIndex];
	if (FInventoryItemInfo* CompositeInfo = ItemInfoTable->FindRow<FInventoryItemInfo>(FName(FString::FromInt(CraftFormula.CompositeId)), TEXT("")))
	{
		TArray<FInventoryItem>& Inventory = Inventories[CompositeInfo->Type - 1];
		for (int i = 0; i < CraftFormula.IngrediantIds.Num(); i++)
		{
			int IngrediantNumber = CraftFormula.IngrediantNumbers[i];
			for (int j = 0; j < Inventory.Num(); j++)
			{
				if (Inventory[j].ItemInfo.Id == CraftFormula.IngrediantIds[i])
				{
					if (Inventory[j].ItemNumber >= IngrediantNumber)
					{
						RemoveItem(CompositeInfo->Type, j, IngrediantNumber);
						break;
					}
					else
					{
						IngrediantNumber -= Inventory[j].ItemNumber;
						RemoveItem(CompositeInfo->Type, j, Inventory[j].ItemNumber);
					}
				}
			}
		}

		FInventoryItem InventoryItem;
		InventoryItem.ItemInfo = *CompositeInfo;
		InventoryItem.ItemNumber = CraftFormula.CompositeNumber;
		AddItem(InventoryItem);
	}
	
}