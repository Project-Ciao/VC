// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemData.h"

#include "Inventory/InventoryComponent.h"

UInventoryComponent* UInventoryItemData::GetInventory() const
{
	return GetTypedOuter<UInventoryComponent>();
}

int32 UInventoryItemData::GetInventorySlotIndex() const
{
	if (UInventoryComponent* Inventory = GetInventory())
	{
		for (int32 i = 0; i < Inventory->GetInventoryItems().Num(); ++i)
		{
			if (Inventory->GetInventoryItems()[i].ItemData == this)
			{
				return i;
			}
		}
	}

	return INDEX_NONE;
}

FInventorySlot* UInventoryItemData::GetInventorySlot() const
{
	int32 InventorySlotIndex = GetInventorySlotIndex();
	if (InventorySlotIndex != INDEX_NONE)
	{
		if (UInventoryComponent* Inventory = GetInventory())
		{
			return &Inventory->GetInventorySlot(InventorySlotIndex);
		}
	}
	return nullptr;
}
