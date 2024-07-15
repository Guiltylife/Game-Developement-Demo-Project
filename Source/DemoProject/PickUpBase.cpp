// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpBase.h"

// Sets default values
APickUpBase::APickUpBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

APickUpBase::APickUpBase(int InItemId, int InItemNumber)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemId = InItemId;
	ItemNumber = InItemNumber;
}

// Called when the game starts or when spawned
void APickUpBase::BeginPlay()
{
	Item.ItemInfo = *ItemTable->FindRow<FInventoryItemInfo>(FName(FString::FromInt(ItemId)), TEXT(""));
	Item.ItemNumber = ItemNumber;

	Super::BeginPlay();
}

// Called every frame
void APickUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

