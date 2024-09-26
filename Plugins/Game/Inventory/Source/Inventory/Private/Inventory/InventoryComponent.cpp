// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInterface.h"
#include "Inventory/InventoryItemData.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Algo/Reverse.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerCategory.h"
#endif

UInventoryComponent::UInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	FillOutInventory(GetMaxInventoryItems());
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventoryComponent, InventoryItems, COND_OwnerOnly);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	FillOutInventory(GetMaxInventoryItems());
}

void UInventoryComponent::OnRegister()
{
	Super::OnRegister();

	FillOutInventory(GetMaxInventoryItems());

	for (int i = 0; i < InventoryItems.Num(); ++i)
	{
		FInventorySlot& InventorySlot = InventoryItems[i];
		if (InventorySlot.InventoryClass.IsNull())
		{
			InventorySlot.EmptySlot();
		}
		else
		{
			if (InventorySlot.ItemAmount <= 0)
			{
				InventorySlot.ItemAmount = 1;
			}

			InventorySlot.ConstructItemData(this);
		}
	}

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CheckSlotsValid();
	}
}

bool UInventoryComponent::AddItemToInventory(FInventorySlot& NewInventoryItem)
{
	checkf(NewInventoryItem.InventoryClass != nullptr && NewInventoryItem.ItemAmount > 0, TEXT("Can't add inventory item with no class or with 0 items"));
	// Make sure the slot has an item data object
	NewInventoryItem.ConstructItemData(this);

	FillOutInventory(GetMaxInventoryItems());

	// Look for inventory slots of a matching class
	for (FInventorySlot& InventorySlot : InventoryItems)
	{
		// If we found a slot that isn't full
		if (InventorySlot.InventoryClass == NewInventoryItem.InventoryClass && !InventorySlot.IsFull())
		{
			InventorySlot.ItemAmount += NewInventoryItem.ItemAmount;
			NewInventoryItem.ItemAmount = 0;

			if (InventorySlot.ItemAmount > InventorySlot.GetSlotMax())
			{
				NewInventoryItem.ItemAmount = InventorySlot.ItemAmount - InventorySlot.GetSlotMax();
				InventorySlot.ItemAmount = InventorySlot.GetSlotMax();
				InventorySlot.OnUIUpdate.Broadcast();
			}
			else
			{
				NewInventoryItem.EmptySlot();
				InventorySlot.OnUIUpdate.Broadcast();
				OnItemAddedToInventory.Broadcast(this, NewInventoryItem);

				CheckSlotsValid();
				return true;
			}
		}
	}

	// Now look for empty slots
	for (int i = 0; i < InventoryItems.Num(); ++i)
	{
		FInventorySlot& InventorySlot = InventoryItems[i];

		// Found one
		if (InventorySlot.InventoryClass == nullptr)
		{
			// Put the item in there
			InventorySlot.CopyItemFrom(NewInventoryItem);
			NewInventoryItem.ItemAmount = 0;

			// If it overfilled
			if (InventorySlot.ItemAmount > InventorySlot.GetSlotMax())
			{
				NewInventoryItem.ItemAmount = InventorySlot.ItemAmount - InventorySlot.GetSlotMax();
				InventorySlot.ItemAmount = InventorySlot.GetSlotMax();
				InventorySlot.OnUIUpdate.Broadcast();
				IInventoryItemInterface::Execute_OnAddedToInventory(InventorySlot.InventoryClass.LoadSynchronous()->GetDefaultObject(), this, i);
			}
			else
			{
				NewInventoryItem.EmptySlot();
				InventorySlot.OnUIUpdate.Broadcast();
				IInventoryItemInterface::Execute_OnAddedToInventory(InventorySlot.InventoryClass.LoadSynchronous()->GetDefaultObject(), this, i);
				OnItemAddedToInventory.Broadcast(this, NewInventoryItem);

				CheckSlotsValid();
				return true;
			}
		}
	}

	// Or just stick it on the end if we have infinite inventory
	if (bIsInfiniteInventory)
	{
		int32 i = InventoryItems.Add(NewInventoryItem);
		NewInventoryItem.EmptySlot();
		IInventoryItemInterface::Execute_OnAddedToInventory(InventoryItems[i].InventoryClass.LoadSynchronous()->GetDefaultObject(), this, i);
		OnItemAddedToInventory.Broadcast(this, InventoryItems[i]);
		InventoryItems[i].OnUIUpdate.Broadcast();

		CheckSlotsValid();
		return true;
	}

	CheckSlotsValid();
	return false;
}

bool UInventoryComponent::K2_AddItemToInventory(const FInventorySlot& Item)
{
	FInventorySlot Copy = Item;
	return AddItemToInventory(Copy);
}

bool UInventoryComponent::AddItemToInventorySlot(FInventorySlot& Item, FIntPoint Slot)
{
	CheckSlotsValid();

	// Make sure the slot has an item data object
	Item.ConstructItemData(this);

	// Make sure the index is correct
	const int32 Index = Coord2DToIndex(Slot, InventorySize);

	if (!bIsInfiniteInventory)
	{
		check(Slot.X < InventorySize.X);
		check(Slot.Y < InventorySize.Y);
		check(Index < GetMaxInventoryItems());

		FillOutInventory(GetMaxInventoryItems());
	}
	else
	{
		FillOutInventory(Index + 1);
		check(Index < InventoryItems.Num());
	}

	if (InventoryItems[Index].InventoryClass == nullptr || InventoryItems[Index].InventoryClass == Item.InventoryClass)
	{
		InventoryItems[Index].InventoryClass = Item.InventoryClass;
		InventoryItems[Index].ItemAmount += Item.ItemAmount;
		InventoryItems[Index].ItemData = Item.ItemData;
		InventoryItems[Index].ItemSaveData = Item.ItemSaveData;
		InventoryItems[Index].OnUIUpdate.Broadcast();

		// If there is too much stuff added/spillover
		if (InventoryItems[Index].ItemAmount > InventoryItems[Index].GetSlotMax())
		{
			Item.ItemAmount = InventoryItems[Index].ItemAmount - InventoryItems[Index].GetSlotMax();
			InventoryItems[Index].ItemAmount = InventoryItems[Index].GetSlotMax();

			CheckSlotsValid();
			return false;
		}
		else
		{
			Item.EmptySlot();
			IInventoryItemInterface::Execute_OnAddedToInventory(InventoryItems[Index].InventoryClass.LoadSynchronous()->GetDefaultObject(), this, Index);

			CheckSlotsValid();
			return true;
		}
	}
	else
	{
		// Can't add an inventory item to a slot of a different type

		CheckSlotsValid();
		return false;
	}
}

int32 UInventoryComponent::GetNumOccupiedSlots() const
{
	int32 OccupiedSlots = 0;
	for (const FInventorySlot& Slot : InventoryItems)
	{
		if (!Slot.IsEmpty())
		{
			OccupiedSlots++;
		}
	}
	return OccupiedSlots;
}

void UInventoryComponent::BroadcastChangedItems()
{
	TArray<int32> ChangedIndicies;

	for (int32 i = 0; i < FMath::Max3(GetMaxInventoryItems(), InventoryItems.Num(), OldInventory.Num()); ++i)
	{
		if (i >= InventoryItems.Num() || i >= OldInventory.Num())
		{
			ChangedIndicies.Add(i);
		}
		else
		{
			if (InventoryItems[i] != OldInventory[i])
			{
				ChangedIndicies.Add(i);
			}
		}
	}

	OldInventory = InventoryItems;

	if (ChangedIndicies.Num() > 0)
	{
		OnInventoryItemsChanged.Broadcast(ChangedIndicies);
		for (int32 Index : ChangedIndicies)
		{
			if (InventoryItems.IsValidIndex(Index))
			{
				InventoryItems[Index].OnUIUpdate.Broadcast();
			}
		}
	}
}

void UInventoryComponent::OnRep_Inventory()
{
	CheckSlotsValid();
	BroadcastChangedItems();
}

bool UInventoryComponent::CanAffordItemRemoval(TArray<FInventorySlot> ItemsToRemove, TArray<FInventorySlot>& ResultingInventory) const
{
	CheckSlotsValid();

	ResultingInventory = InventoryItems;

	// Reverse the array so that the last items are removed first
	Algo::Reverse(ResultingInventory);
	
	for (FInventorySlot& RemoveSlot : ItemsToRemove)
	{
		// This should probably be reverse iterated so that items at the end of the inventory are removed first,
		// but whatever.
		for (FInventorySlot& InventorySlot : ResultingInventory)
		{
			if (!InventorySlot.IsEmpty() && InventorySlot.InventoryClass == RemoveSlot.InventoryClass)
			{
				InventorySlot.ItemAmount = InventorySlot.ItemAmount - RemoveSlot.ItemAmount;
				RemoveSlot.ItemAmount = 0;

				if (InventorySlot.ItemAmount <= 0)
				{
					RemoveSlot.ItemAmount = -InventorySlot.ItemAmount;
					//InventorySlot.EmptySlot();
				}

				if (RemoveSlot.ItemAmount == 0)
				{
					break;
				}
			}
		}
	}

	Algo::Reverse(ResultingInventory);

	// Check to make sure that all the items were removed
	for (FInventorySlot& RemoveSlot : ItemsToRemove)
	{
		if (RemoveSlot.ItemAmount > 0)
		{
			return false;
		}
	}

	return true;
}

int32 UInventoryComponent::GetItemCount(TSoftClassPtr<UObject> InventoryClass) const
{
	CheckSlotsValid();

	int32 Count = 0;

	for (const FInventorySlot& InventorySlot : InventoryItems)
	{
		if (InventorySlot.InventoryClass == InventoryClass)
		{
			Count += InventorySlot.ItemAmount;
		}
	}

	return Count;
}

void UInventoryComponent::UseInventoryItem(bool bReplicate, int32 Index)
{
	CheckSlotsValid();

	if (bReplicate && !GetOwner()->HasAuthority())
	{
		Server_UseInventoryItem(Index);
	}

	if (InventoryItems.IsValidIndex(Index) && InventoryItems[Index].InventoryClass)
	{
		UClass* ItemClass = InventoryItems[Index].InventoryClass.LoadSynchronous();
		UObject* ItemCDO = ItemClass->GetDefaultObject();
		if (ItemClass->ImplementsInterface(UInventoryItemInterface::StaticClass()) && ItemCDO)
		{
			IInventoryItemInterface::Execute_UseInInventory(ItemCDO, this, Index);
		}
	}
}

void UInventoryComponent::Server_UseInventoryItem_Implementation(int32 Index)
{
	CheckSlotsValid();
	UseInventoryItem(false, Index);
}

bool UInventoryComponent::Server_UseInventoryItem_Validate(int32 Index)
{
	return true;
}

bool UInventoryComponent::RemoveItemAmountFromSlot(int32 Slot, int32 Amount)
{
	CheckSlotsValid();

	checkf(InventoryItems.IsValidIndex(Slot), TEXT("UInventoryComponent::RemoveItemAmountFromSlot slot index isn't valid!"));
	if (InventoryItems[Slot].ItemAmount >= Amount)
	{
		InventoryItems[Slot].ItemAmount -= Amount;

		if (InventoryItems[Slot].ItemAmount == 0)
		{
			InventoryItems[Slot].EmptySlot();
		}
		else
		{
			InventoryItems[Slot].OnUIUpdate.Broadcast();
		}

		return true;
	}
	else
	{
		return false;
	}
}

FText UInventoryComponent::GetInventoryName_Implementation()
{
	return FText::GetEmpty();
}

bool UInventoryComponent::RemoveItemAmountByClass(TSoftClassPtr<UObject> ItemClass, int32 Amount)
{
	CheckSlotsValid();

	for (FInventorySlot& InventorySlot : InventoryItems)
	{
		if (InventorySlot.InventoryClass == ItemClass)
		{
			InventorySlot.ItemAmount = InventorySlot.ItemAmount - Amount;
			
			// If the amount removed was greater than the amount in the slot
			if (InventorySlot.ItemAmount < 0)
			{
				Amount = -InventorySlot.ItemAmount;
				InventorySlot.EmptySlot();
			}
			else
			{
				// if the amount in the slot was greater or equal to the amount removed
				if (InventorySlot.ItemAmount == 0)
				{
					InventorySlot.EmptySlot();
				}
				Amount = 0;
				InventorySlot.OnUIUpdate.Broadcast();
				break;
			}
		}
	}

	return Amount == 0;
}

void UInventoryComponent::RemoveItems(TArray<FInventorySlot> ItemsToRemove)
{
	CheckSlotsValid();

	for (FInventorySlot& RemoveSlot : ItemsToRemove)
	{
		for (int32 i = InventoryItems.Num() - 1; i >= 0; i--)
		{
			FInventorySlot& InventorySlot = InventoryItems[i];

			if (!InventorySlot.IsEmpty() && InventorySlot.InventoryClass == RemoveSlot.InventoryClass)
			{
				InventorySlot.ItemAmount = InventorySlot.ItemAmount - RemoveSlot.ItemAmount;
				RemoveSlot.ItemAmount = 0;

				if (InventorySlot.ItemAmount <= 0)
				{
					RemoveSlot.ItemAmount = -InventorySlot.ItemAmount;
					InventorySlot.EmptySlot();
				}

				if (RemoveSlot.ItemAmount == 0)
				{
					break;
				}
			}
		}
	}

	// Check to make sure that all the items were removed
	for (FInventorySlot& RemoveSlot : ItemsToRemove)
	{
		if (RemoveSlot.ItemAmount > 0)
		{
			checkNoEntry();
		}
	}
}

bool UInventoryComponent::EmptySlotByIndex(int32 Slot)
{
	CheckSlotsValid();

	checkf(InventoryItems.IsValidIndex(Slot), TEXT("UInventoryComponent::EmptySlotByIndex slot index isn't valid!"));
	if (InventoryItems[Slot].ItemAmount > 0)
	{
		InventoryItems[Slot].EmptySlot();
		return true;
	}

	return false;
}

FInventorySlot UInventoryComponent::ConstructSlotFromClass(TSoftClassPtr<UObject> ItemClass, int32 Amount)
{
	if (!ItemClass.IsNull() && Amount > 0)
	{
		FInventorySlot OutSlot;
		OutSlot.InventoryClass = ItemClass;
		OutSlot.ItemAmount = Amount;
		OutSlot.ConstructItemData(this);
		return OutSlot;
	}
	return FInventorySlot();
}

void UInventoryComponent::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving() && Ar.ArIsSaveGame)
	{
		for (int i = 0; i < InventoryItems.Num(); ++i)
		{
			FInventorySlot& InventorySlot = InventoryItems[i];
			// Save all item data
			InventorySlot.SaveItemData();
		}
	}
	
	TArray<FInventorySlot> PreSerializedInventoryItems = InventoryItems;

	Super::Serialize(Ar);

	if (Ar.IsLoading() && Ar.ArIsSaveGame)
	{
		// Retain the size of the array after loading
		int32 NumItems = FMath::Max(PreSerializedInventoryItems.Num(), InventoryItems.Num());
		FillOutInventory(NumItems);

		for (int i = 0; i < NumItems; ++i)
		{
			FInventorySlot& InventorySlot = InventoryItems[i];

			// Make sure the delegate before saving is still valid even after the array is messed around with
			if (PreSerializedInventoryItems.IsValidIndex(i))
			{
				InventorySlot.OnUIUpdate = PreSerializedInventoryItems[i].OnUIUpdate;
			}

			if (InventorySlot.InventoryClass)
			{
				InventorySlot.ConstructItemData(this);
				InventorySlot.LoadItemData();
				IInventoryItemInterface::Execute_OnAddedToInventory(InventorySlot.InventoryClass.LoadSynchronous()->GetDefaultObject(), this, i);
			}
			InventorySlot.OnUIUpdate.Broadcast();
		}

		OldInventory = PreSerializedInventoryItems;
		BroadcastChangedItems();
	}
}

FInventorySlot* UInventoryComponent::GetSlotByClass(TSoftClassPtr<UObject> ItemClass)
{
	for (FInventorySlot& Slot : InventoryItems)
	{
		if (Slot.InventoryClass == ItemClass)
		{
			return &Slot;
		}
	}
	return nullptr;
}

void UInventoryComponent::BindInventoryUIUpdateDelegate(int32 InventoryIndex, const FOnInventoryItemUIUpdateDelegate& Delegate)
{
	if (InventoryItems.IsValidIndex(InventoryIndex))
	{
		InventoryItems[InventoryIndex].OnUIUpdate.AddUnique(Delegate);
	}
}

void UInventoryComponent::CallInventoryUIUpdateDelegate(int32 InventoryIndex)
{
	if (InventoryItems.IsValidIndex(InventoryIndex))
	{
		InventoryItems[InventoryIndex].OnUIUpdate.Broadcast();
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventorySlot& Slot : InventoryItems)
	{
		if (Slot.ItemData)
		{
			bWroteSomething |= Channel->ReplicateSubobject(Slot.ItemData, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

bool UInventoryComponent::GetInventorySlot(int32 Index, FInventorySlot& OutSlot)
{
	if (InventoryItems.IsValidIndex(Index))
	{
		OutSlot = InventoryItems[Index];
		return true;
	}
	else
	{
		OutSlot = FInventorySlot();
		return false;
	}
}

void UInventoryComponent::CheckSlotsValid() const
{
	TSet<FGuid> SlotIDs;
	SlotIDs.Reserve(InventoryItems.Num());

	for (const FInventorySlot& Slot : InventoryItems)
	{
		Slot.CheckValid();

		if (Slot.ItemGuid.IsValid())
		{
			bool bAlreadyInSet = false;
			SlotIDs.Add(Slot.ItemGuid, &bAlreadyInSet);
			ensure(bAlreadyInSet == false);
		}
	}
}

bool UInventoryComponent::GetSlotByGUID(FGuid ID, FInventorySlot& OutSlot, int32& OutIndex)
{
	OutIndex = 0;
	for (FInventorySlot& Slot : InventoryItems)
	{
		if (Slot.ItemGuid == ID)
		{
			OutSlot = Slot;
			return true;
		}
		OutIndex++;
	}
	return false;
}

void UInventoryComponent::FillOutInventory(int32 NumItems)
{
	// Add in defaulted inventory items
	if (InventoryItems.Num() < NumItems)
	{
		InventoryItems.AddDefaulted(NumItems - InventoryItems.Num());
		check(InventoryItems.Num() == NumItems);
	}
}

void UInventoryComponent::RunItemAction(int32 InventoryIndex, const FInventoryItemActionDelegate& Action, UObject* ActionData)
{
	Action.ExecuteIfBound(this, InventoryIndex, ActionData);
}

#if WITH_GAMEPLAY_DEBUGGER
void UInventoryComponent::DescribeSelfToGameplayDebugger(FGameplayDebuggerCategory* DebuggerCategory) const
{
	if (DebuggerCategory == nullptr)
		return;

	DebuggerCategory->AddTextLine(FString::Printf(TEXT("{yellow}Inventory Component (%s):"), *GetName()));

	if (bIsInfiniteInventory)
	{
		DebuggerCategory->AddTextLine("\t{yellow}Inventory size: INFINITE");

		DebuggerCategory->AddTextLine(FString::Printf(TEXT("\t{yellow}Num occupied item slots: %d"), GetNumOccupiedSlots()));
	}
	else
	{
		DebuggerCategory->AddTextLine(FString::Printf(TEXT("\t{yellow}Inventory size: %d x %d"),
			InventorySize.X,
			InventorySize.Y)
		);

		DebuggerCategory->AddTextLine(FString::Printf(TEXT("\t{yellow}Num occupied item slots: %d / %d"), GetNumOccupiedSlots(), GetMaxInventoryItems()));
	}

	DebuggerCategory->AddTextLine("\t{yellow}Items:");
	for (int i = 0; i < InventoryItems.Num(); ++i)
	{
		const FInventorySlot& InventorySlot = InventoryItems[i];
		if (InventorySlot.InventoryClass == nullptr && InventorySlot.ItemAmount == 0)
		{
			DebuggerCategory->AddTextLine(FString::Printf(TEXT("\t\t {yellow}Slot %d: EMPTY"), i));
		}
		else
		{
			DebuggerCategory->AddTextLine(
				FString::Printf(TEXT("\t\t {yellow}Slot %d: %s (%d / %d)"),
					i,
					InventorySlot.InventoryClass ? *InventorySlot.InventoryClass->GetName() : TEXT("NONE"),
					InventorySlot.ItemAmount,
					InventorySlot.GetSlotMax())
			);
		}
	}
}
#endif