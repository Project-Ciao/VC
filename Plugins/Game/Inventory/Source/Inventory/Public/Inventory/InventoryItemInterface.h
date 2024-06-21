// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inventory/InventoryTypes.h"
#include "InventoryItemInterface.generated.h"

class ACharacter;
class UInventoryComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INVENTORY_API IInventoryItemInterface
{
	GENERATED_BODY()

public:
	/** This function will only be called through the CDO */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		void UseInInventory(UInventoryComponent* InventoryComponent, int32 InventoryIndex) const;

	/** This function will only be called through the CDO */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		TArray<FInventoryItemAction> GetInventoryItemActions(UInventoryComponent* InventoryComponent, int32 InventoryIndex) const;

	/** This function will only be called through the CDO */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		FInventoryIcon GetInventoryIcon(UInventoryComponent* InventoryComponent, int32 InventoryIndex) const;

	/** This function will only be called through the CDO */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		FText GetInventoryItemName(UInventoryComponent* InventoryComponent, int32 InventoryIndex) const;

	/** This function will only be called through the CDO */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		FText GetInventoryItemDescription(UInventoryComponent* InventoryComponent, int32 InventoryIndex) const;

	/** This function will only be called through the CDO */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		int32 GetMaxStackSize() const;

	/** This function will only be called through the CDO */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		void OnAddedToInventory(UInventoryComponent* InventoryComponent, int32 InventoryIndex) const;

	/** This function will only be called through the CDO */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		void OnRemovedFromInventory(UInventoryComponent* InventoryComponent, int32 InventoryIndex) const;

	/** This function will only be called through the CDO. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory Item Interface")
		TSubclassOf<UObject> GetSaveObjectClass() const;
};
