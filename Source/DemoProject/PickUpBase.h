// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "PickUpBase.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanStacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxStackingNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInventoryItemInfo ItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ItemNumber;
};

USTRUCT(BlueprintType)
struct FCraftFormula : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> IngrediantIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> IngrediantNumbers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CompositeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CompositeNumber;
};

USTRUCT(BlueprintType)
struct FItemEffect : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<int> CharacterBuffs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CharacterHealthEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CharacterEnergyEffect;
};

UCLASS()
class DEMOPROJECT_API APickUpBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpBase();

	APickUpBase(int InItemId, int InItemNumber);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* ItemTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnTrigger", Meta = (ExposeOnSpawn = true))
	int ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnTrigger", Meta = (ExposeOnSpawn = true))
	int ItemNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FInventoryItem Item;
};
