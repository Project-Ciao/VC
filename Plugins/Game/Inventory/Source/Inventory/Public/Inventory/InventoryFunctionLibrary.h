// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Inventory Item Action")
		static FInventoryItemAction MakeInventoryItemAction(FText ActionName, class USoundBase* ActionUISound, const FInventoryItemActionDelegate& Action, UObject* ActionData);
};
