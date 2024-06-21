// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "InventoryTypes.generated.h"

class UInventoryComponent;
class UInventoryItemData;

DECLARE_DYNAMIC_DELEGATE(FOnInventoryItemUIUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryItemUIUpdate);

USTRUCT(BlueprintType)
struct INVENTORY_API FInventorySlot
{
	GENERATED_BODY()

	FInventorySlot(TSoftClassPtr<UObject> InInventoryClass, int32 InItemAmount)
		: InventoryClass(InInventoryClass), ItemAmount(InItemAmount), ItemData(nullptr), ItemSaveData(), ItemGuid(FGuid::NewGuid())
	{

	}

	FInventorySlot()
		: InventoryClass(nullptr), ItemAmount(0), ItemData(nullptr), ItemSaveData(), ItemGuid()
	{

	}

	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = "Inventory Slot", meta = (MustImplement = "InventoryItemInterface"))
		TSoftClassPtr<UObject> InventoryClass;

	UPROPERTY(EditAnywhere, SaveGame, BlueprintReadWrite, Category = "Inventory Slot", meta = (ClampMin = "1", UIMin = "1"))
		int32 ItemAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category = "Inventory Slot")
		UInventoryItemData* ItemData;

	UPROPERTY(BlueprintAssignable, Category = "Inventory Slot")
		FOnInventoryItemUIUpdate OnUIUpdate;

	UPROPERTY(SaveGame, NotReplicated)
		TArray<uint8> ItemSaveData;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Inventory Slot")
		FGuid ItemGuid;

	UPROPERTY(Transient)
		TWeakObjectPtr<UInventoryComponent> OuterInventory;

public:
	int32 GetSlotMax() const;

	bool IsFull() const;
	bool IsEmpty() const;

	void EmptySlot();
	void ConstructItemData(UInventoryComponent* InOuterInventory);

	void SaveItemData();
	void LoadItemData();

	// Returns the item slot split into multiple slots based on the max stack amount
	TArray<FInventorySlot> SplitOverflowIntoMultipleSlots() const;

	TArray<FInventorySlot> SplitIntoMultipleSlots(int32 MaxPerSlot) const;

	bool operator==(const FInventorySlot& Other) const
	{
		return InventoryClass == Other.InventoryClass
			&& ItemAmount == Other.ItemAmount;
	}

	bool operator!=(const FInventorySlot& Other) const
	{
		return !(*this == Other);
	}

	// Swaps two slots without swapping OnUIUpdate.
	// You should probably call OnUIUpdate for both slots after this
	static void Swap(FInventorySlot& Lhs, FInventorySlot& Rhs);

	// Checks if the slot is in a valid state
	void CheckValid() const;
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FInventoryItemActionDelegate, UInventoryComponent*, InventoryComponent, int32, InventoryIndex, UObject*, ActionData);

USTRUCT(BlueprintType, meta=(HasNativeMake="Inventory.InventoryFunctionLibrary.MakeInventoryItemAction"))
struct FInventoryItemAction
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item Action")
		FText ActionName;

	// UI Sound that is played when the action is ran
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item Action")
		USoundBase* ActionUISound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item Action")
		FInventoryItemActionDelegate Action;

	// Extra data used for an action since variables can't be captured in the action delegate
	// Note: this object is not replicated to the server
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item Action")
		UObject* ActionData;
};

USTRUCT(BlueprintType)
struct FInventoryIcon
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Icon")
		TSoftObjectPtr<class UMaterialInterface> IconMaterial;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory Icon")
		TSoftClassPtr<class UUserWidget> IconWidget;
};

static int32 Coord2DToIndex(FIntPoint Coordinate, FIntPoint Size2D)
{
	return Coordinate.X + (Coordinate.Y * Size2D.X);
}

static FIntPoint IndexToCoord2D(int32 Index, FIntPoint Size2D)
{
	return FIntPoint(Index % Size2D.X, Index / Size2D.X);
}