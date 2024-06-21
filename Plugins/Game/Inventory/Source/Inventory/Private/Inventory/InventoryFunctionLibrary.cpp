// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryFunctionLibrary.h"

FInventoryItemAction UInventoryFunctionLibrary::MakeInventoryItemAction(FText ActionName, USoundBase* ActionUISound, const FInventoryItemActionDelegate& Action, UObject* ActionData)
{
	return FInventoryItemAction({ ActionName, ActionUISound, Action, ActionData });
}
