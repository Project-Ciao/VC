// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/NetDriver.h"
#include "InventoryItemData.generated.h"

class UInventoryComponent;

/**
 * Data for an item stored in an inventory slot. Supports replication, gets serialized and deserialized by the inventory component
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Inventory))
class INVENTORY_API UInventoryItemData : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void PostInitialize() {};

	virtual UWorld* GetWorld() const override
	{
		if (const UObject* MyOuter = GetOuter())
		{
			return MyOuter->GetWorld();
		}
		return nullptr;
	}

	UFUNCTION(BlueprintPure, Category = "Inventory|Item Data")
	UInventoryComponent* GetInventory() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Item Data")
	int32 GetInventorySlotIndex() const;

	struct FInventorySlot* GetInventorySlot() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Item Data")
	AActor* GetOwningActor() const
	{
		return GetTypedOuter<AActor>();
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		// Add any Blueprint properties
		// This is not required if you do not want the class to be "Blueprintable"
		if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
		{
			BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
		}
	}

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override
	{
		check(GetOuter() != nullptr);
		return GetOuter()->GetFunctionCallspace(Function, Stack);
	}

	// Call "Remote" (aka, RPC) functions through the actors NetDriver
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, struct FOutParmRec* OutParms, FFrame* Stack) override
	{
		check(!HasAnyFlags(RF_ClassDefaultObject));
		AActor* Owner = GetOwningActor();
		UNetDriver* NetDriver = Owner->GetNetDriver();
		if (NetDriver)
		{
			NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
			return true;
		}
		return false;
	}

	void Initialize()
	{
		if (!bIsInitialized)
		{
			bIsInitialized = true;
			PostInitialize();
		}
	}

private:
	bool bIsInitialized = false;
};
