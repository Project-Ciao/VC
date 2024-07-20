// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemsChanged, const TArray<int32>&, ChangedIndices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedToInventory, UInventoryComponent*, Inventory, const FInventorySlot&, Item);

UCLASS( BlueprintType, Blueprintable, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent) )
class INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory Component")
		bool bIsInfiniteInventory;

	// This does not limit the size of the inventory if it is infinite
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Inventory Component")
		FIntPoint InventorySize;

	virtual void BeginPlay() override;
	virtual void OnRegister() override;

	// Adds an item to the inventory, returns true if there was no overflow
	bool AddItemToInventory(FInventorySlot& Item);
	
	// Adds an item to the inventory, returns true if there was no overflow
	UFUNCTION(BlueprintCallable, Category = "Inventory Component", meta = (DisplayName = "Add Item To Inventory", AutoCreateRefTerm = "Item"))
		bool K2_AddItemToInventory(const FInventorySlot& Item);

	UPROPERTY(BlueprintAssignable, Category = "Inventory Component")
		FOnItemAddedToInventory OnItemAddedToInventory;

	// Adds an item to the inventory slot, returns true if there was no overflow
	bool AddItemToInventorySlot(FInventorySlot& Item, FIntPoint Slot);

	// Adds an item to the inventory slot, returns true if there was no overflow
	UFUNCTION(BlueprintCallable, Category = "Inventory Component", meta = (DisplayName = "Add Item To Inventory Slot", AutoCreateRefTerm = "Item"))
		bool K2_AddItemToInventorySlot(UPARAM(ref) FInventorySlot& Item, FIntPoint Slot) { return AddItemToInventorySlot(Item, Slot); };

	UFUNCTION(BlueprintPure, Category = "Inventory Component")
		int32 GetMaxInventoryItems() const { return InventorySize.X * InventorySize.Y; };

	UFUNCTION(BlueprintPure, Category = "Inventory Component")
		int32 GetNumOccupiedSlots() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory Component")
		FOnInventoryItemsChanged OnInventoryItemsChanged;

	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		void BroadcastChangedItems();

	UFUNCTION()
		void OnRep_Inventory();

	// Returns true if the inventory contains these items and they can be removed
	UFUNCTION(BlueprintPure, Category = "Inventory Component")
		bool CanAffordItemRemoval(TArray<FInventorySlot> ItemsToRemove, TArray<FInventorySlot>& ResultingInventory) const;

	UFUNCTION(BlueprintPure, Category = "Inventory Component")
		int32 GetItemCount(TSoftClassPtr<UObject> InventoryClass) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		void UseInventoryItem(bool bReplicate, int32 Index);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_UseInventoryItem(int32 Index);

	// Returns true if it was able to remove the amount from the slot
	bool RemoveItemAmountFromSlot(int32 Slot, int32 Amount);

	// Returns true if it was able to remove the amount from the slot
	UFUNCTION(BlueprintCallable, Category = "Inventory Component", meta = (DisplayName = "Remove Item Amount From Slot"))
		bool K2_RemoveItemAmountFromSlot(int32 Slot, int32 Amount) { return RemoveItemAmountFromSlot(Slot, Amount); };

	// Returns true if it was able to remove the total amount
	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		bool RemoveItemAmountByClass(TSoftClassPtr<UObject> ItemClass, int32 Amount);

	// Removes the items from the inventory.
	// Will throw if it wasn't able to remove all items, so check
	// with CanAffordItemRemoval before calling this function
	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		void RemoveItems(TArray<FInventorySlot> ItemsToRemove);

	// Returns true if the slot had an item
	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		bool EmptySlotByIndex(int32 Slot);

	UFUNCTION(BlueprintNativeEvent, Category = "Inventory Component")
		FText GetInventoryName();

	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		FInventorySlot ConstructSlotFromClass(TSoftClassPtr<UObject> ItemClass, int32 Amount = 1);

	virtual void Serialize(FArchive& Ar) override;

	// Returns the first slot that matches this class
	FInventorySlot* GetSlotByClass(TSoftClassPtr<UObject> ItemClass);

#if WITH_GAMEPLAY_DEBUGGER
	virtual void DescribeSelfToGameplayDebugger(class FGameplayDebuggerCategory* DebuggerCategory) const;
#endif // WITH_GAMEPLAY_DEBUGGER

	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		void RunItemAction(int32 InventoryIndex, const FInventoryItemActionDelegate& Action, UObject* ActionData = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		void BindInventoryUIUpdateDelegate(int32 InventoryIndex, const FOnInventoryItemUIUpdateDelegate& Delegate);

	UFUNCTION(BlueprintCallable, Category = "Inventory Component")
		void CallInventoryUIUpdateDelegate(int32 InventoryIndex);

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	const TArray<FInventorySlot>& GetInventoryItems() const { return InventoryItems; }

	FInventorySlot& GetInventorySlot(int32 Index) { return InventoryItems[Index]; }

	UFUNCTION(BlueprintPure, Category = "Inventory Component")
		bool GetInventorySlot(int32 Index, FInventorySlot& OutSlot);

	// check()s all slots to see if they are valid (have an inventory class + amount, have unique GUIDs)
	virtual void CheckSlotsValid() const;

	UFUNCTION(BlueprintPure, Category = "Inventory Component")
		bool GetSlotByGUID(FGuid ID, FInventorySlot& OutSlot, int32& OutIndex);

protected:
	// Fills out the inventory with default initialized items
	void FillOutInventory(int32 NumItems);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, SaveGame, ReplicatedUsing = OnRep_Inventory, Category = "Inventory Component")
		TArray<FInventorySlot> InventoryItems;

	UPROPERTY(Transient)
		TArray<FInventorySlot> OldInventory;
};
