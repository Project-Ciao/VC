// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryTypes.h"

#include "Inventory/InventoryItemInterface.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItemData.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

int32 FInventorySlot::GetSlotMax() const
{
	const int32 SlotMax = InventoryClass ? IInventoryItemInterface::Execute_GetMaxStackSize(InventoryClass.LoadSynchronous()->GetDefaultObject()) : 0;
	return FMath::Max(1, SlotMax);
}

bool FInventorySlot::IsFull() const
{
	if (InventoryClass == nullptr || ItemAmount == 0)
	{
		return false;
	}
	else
	{
		return ItemAmount >= GetSlotMax();
	}
}

bool FInventorySlot::IsEmpty() const
{
	return ItemAmount == 0 || InventoryClass == nullptr;
}

void FInventorySlot::EmptySlot()
{
	InventoryClass = nullptr;
	ItemAmount = 0;
	ItemData = nullptr;
	ItemSaveData.Empty();
	ItemGuid.Invalidate();

	OnUIUpdate.Broadcast();
}

void FInventorySlot::ConstructItemData(UInventoryComponent* InOuterInventory)
{
	OuterInventory = InOuterInventory;
	if (InventoryClass.IsNull())
	{
		return;
	}

	if (OuterInventory->GetOwnerRole() != ENetRole::ROLE_Authority)
	{
		return;
	}

	UClass* SaveDataClass = IInventoryItemInterface::Execute_GetSaveObjectClass(InventoryClass.LoadSynchronous()->GetDefaultObject());

	if (ItemData != nullptr && ItemData->GetClass() != SaveDataClass)
	{
		ItemData = nullptr;
	}

	if (SaveDataClass && ItemData == nullptr)
	{
		ItemData = NewObject<UInventoryItemData>(InOuterInventory, SaveDataClass);
		LoadItemData();
		ItemData->Initialize();
	}

	if (!ItemGuid.IsValid())
	{
		ItemGuid = FGuid::NewGuid();
	}
}

namespace
{
	struct FSaveGameArchive : public FObjectAndNameAsStringProxyArchive
	{
		FSaveGameArchive(FArchive& InInnerArchive)
			: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
		{
			ArIsSaveGame = true;
		}
	};
}

void FInventorySlot::SaveItemData()
{
	if (ItemData)
	{
		FMemoryWriter MemoryWriter(ItemSaveData, true);
		FSaveGameArchive Ar(MemoryWriter);
		ItemData->Serialize(Ar);
	}
	else
	{
		ItemSaveData.Empty();
	}
}

void FInventorySlot::LoadItemData()
{
	if (ItemData)
	{
		FMemoryReader MemoryReader(ItemSaveData, true);
		FSaveGameArchive Ar(MemoryReader);
		ItemData->Serialize(Ar);
	}
}

TArray<FInventorySlot> FInventorySlot::SplitOverflowIntoMultipleSlots() const
{
	if (!IsFull())
	{
		return { *this };
	}
	else
	{
		return SplitIntoMultipleSlots(GetSlotMax());
	}
}

TArray<FInventorySlot> FInventorySlot::SplitIntoMultipleSlots(int32 MaxPerSlot) const
{
	const auto Result = std::div(ItemAmount, MaxPerSlot);
	const int32 Remainder = Result.rem;
	if (Remainder == 0)
	{
		const int32 NumSlots = Result.quot;

		TArray<FInventorySlot> Slots;
		Slots.Reserve(NumSlots);

		for (int32 i = 0; i < NumSlots; ++i)
		{
			Slots[i].InventoryClass = InventoryClass;
			Slots[i].ItemAmount = MaxPerSlot;
		}

		return Slots;
	}
	else
	{
		const int32 NumSlots = Result.quot + 1;

		TArray<FInventorySlot> Slots;
		Slots.Reserve(NumSlots);

		for (int32 i = 0; i < NumSlots; ++i)
		{
			Slots[i].InventoryClass = InventoryClass;
			Slots[i].ItemAmount = (i == NumSlots - 1) ? Remainder : MaxPerSlot;
		}

		return Slots;
	}
}

void FInventorySlot::Swap(FInventorySlot& Lhs, FInventorySlot& Rhs)
{
	::Swap(Lhs.InventoryClass, Rhs.InventoryClass);
	::Swap(Lhs.ItemAmount, Rhs.ItemAmount);
	::Swap(Lhs.ItemData, Rhs.ItemData);
	::Swap(Lhs.ItemSaveData, Rhs.ItemSaveData);
	::Swap(Lhs.ItemGuid, Rhs.ItemGuid);
}

void FInventorySlot::CheckValid() const
{
#if WITH_EDITOR
	if (InventoryClass.IsNull())
	{
		check(ItemAmount == 0);
		check(ItemData == nullptr);
		check(ItemSaveData.IsEmpty());
		check(!ItemGuid.IsValid());
	}
	else
	{
		check(ItemAmount != 0);
		if (OuterInventory.IsValid() && OuterInventory->GetOwnerRole() == ENetRole::ROLE_Authority)
		{
			check(ItemGuid.IsValid());
		}
	}
#endif
}

