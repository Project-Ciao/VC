// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentHolderComponent.h"

#include "Equipment/EquipableItemComponent.h"
#include "Net/UnrealNetwork.h"

UE_DISABLE_OPTIMIZATION
static bool IsClassReplicated(TSubclassOf<AActor> Class)
{
	AActor* ChildClassCDO = (Class ? Class->GetDefaultObject<AActor>() : nullptr);
	const bool bChildActorClassReplicated = ChildClassCDO && ChildClassCDO->GetIsReplicated();
	return bChildActorClassReplicated;
}

UEquipmentHolderComponent::UEquipmentHolderComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	//bAllowReregistration = false;
	SetIsReplicatedByDefault(true);

	bEquipmentActorsAreTransient = true;
}

void UEquipmentHolderComponent::InitializeComponent()
{
	Super::InitializeComponent();

	check(!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject));

	EquipmentSlots.OwningObject = this;

	RefreshEquipment();
}

void UEquipmentHolderComponent::OnRegister()
{
	Super::OnRegister();

	check(!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject));

	EquipmentSlots.OwningObject = this;

	// Call refresh equipment one frame after registration
	AsyncTask(ENamedThreads::GameThread, [WeakThis = TWeakObjectPtr<ThisClass>(this)]() {
		if (WeakThis.IsValid())
		{
			WeakThis->RefreshEquipment();
		}
	});
}

void UEquipmentHolderComponent::OnUnregister()
{
	Super::OnUnregister();

	check(!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject));

	//if (GIsReconstructingBlueprintInstances)
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("Actor is being destroyed!!!!!"));
	//	return;
	//}

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment)
		{
			SetSlotEquipment(Slot, (AActor*)nullptr, true);
		}
	}
}

void UEquipmentHolderComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentHolderComponent, EquipmentSlots);
}

FEquipmentSlot& UEquipmentHolderComponent::GetOrCreateSlotById(FName SlotId)
{
	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return Slot;
		}
	}

	return EquipmentSlots.CreateSlot(SlotId);
}

bool UEquipmentHolderComponent::GetEquipmentSlot(FName SlotID, FEquipmentSlot& Slot)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == SlotID)
		{
			Slot = EquipSlot;
			return true;
		}
	}

	return false;
}

USceneComponent* UEquipmentHolderComponent::GetEquipmentSlotAttachComponent(FName SlotID) const
{
	FEquipmentSlot Slot;
	if (const_cast<UEquipmentHolderComponent*>(this)->GetEquipmentSlot(SlotID, Slot))
	{
		return Slot.GetAttachComponent(GetOwner());
	}
	return nullptr;
}

void UEquipmentHolderComponent::SetEquipmentSlot(const FEquipmentSlot& Slot)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == Slot.SlotId)
		{
			EquipSlot = Slot;
			return;
		}
	}

	EquipmentSlots.Slots.Add(Slot);
}

bool UEquipmentHolderComponent::SetSlotEquipment(FName SlotID, TSubclassOf<AActor> Class, bool bDestroyOldEquipment, bool bUpdateDefaultEquipmentClass)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == SlotID)
		{
			SetSlotEquipment(EquipSlot, Class, bDestroyOldEquipment, bUpdateDefaultEquipmentClass);
			return true;
		}
	}

	return false;
}

bool UEquipmentHolderComponent::SetEquipmentSlotAttachComponent(FName SlotID, USceneComponent* AttachComponent)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == SlotID)
		{
			EquipSlot.AttachComponent.OverrideComponent = AttachComponent;
			if (EquipSlot.Equipment && AttachComponent)
			{
				EquipSlot.Equipment->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, EquipSlot.AttachBone);
			}
			return true;
		}
	}

	return false;
}

void UEquipmentHolderComponent::SetSlotEquipment(FEquipmentSlot& Slot, AActor* NewEquipment, bool bDestroyOldEquipment, bool bUpdateDefaultEquipmentClass)
{
	const TSubclassOf<AActor> CurrentEquipmentClass = Slot.GetSlotClass();
	const TSubclassOf<AActor> NewEquipmentClass = NewEquipment ? NewEquipment->GetClass() : nullptr;

	if (NewEquipment != Slot.Equipment || CurrentEquipmentClass != NewEquipmentClass)
	{
		AActor* OldEquipment = Slot.Equipment;
		if (bUpdateDefaultEquipmentClass && (HasBegunPlay() || GetOwner()->IsRunningUserConstructionScript()))
		{
			Slot.DefaultEquipmentClass = NewEquipment ? NewEquipment->GetClass() : nullptr;
		}
		Slot.Equipment = NewEquipment;

		if (OldEquipment)
		{
			PerformUnequip(Slot, OldEquipment, bDestroyOldEquipment);
		}

		if (NewEquipment)
		{
			PerformEquip(Slot, OldEquipment, NewEquipment);
		}

		GetEquipmentSlots().MarkSlotDirty(Slot);
	}
}

void UEquipmentHolderComponent::SetSlotEquipment(FEquipmentSlot& Slot, TSubclassOf<AActor> NewEquipmentClass, bool bDestroyOldEquipment, bool bUpdateDefaultEquipmentClass)
{
	const TSubclassOf<AActor> CurrentEquipmentClass = Slot.GetSlotClass();

	if (NewEquipmentClass != CurrentEquipmentClass)
	{
		AActor* NewEquipment = SpawnEquipmentClass(Slot, NewEquipmentClass);

		SetSlotEquipment(Slot, NewEquipment, bDestroyOldEquipment, bUpdateDefaultEquipmentClass);
	}
}

void UEquipmentHolderComponent::RespawnSlotActor(FName SlotID)
{
	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.SlotId == SlotID && GetDefaultEquipmentClass(Slot))
		{
			SetSlotEquipment(Slot, GetDefaultEquipmentClass(Slot));
			return;
		}
	}
}

bool UEquipmentHolderComponent::UnequipActor(AActor* Actor, bool bDestroyActor)
{
	if (Actor == nullptr)
		return false;

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment == Actor)
		{
			SetSlotEquipment(Slot, (AActor*)nullptr, bDestroyActor);
			return true;
		}
	}

	return false;
}

bool UEquipmentHolderComponent::HasEquipment(AActor* Equipment) const
{
	for (const FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment == Equipment)
		{
			return true;
		}
	}

	return false;
}

bool UEquipmentHolderComponent::GetSlotFromEquipment(const AActor* Equipment, FEquipmentSlot& InSlot)
{
	for (const FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment == Equipment)
		{
			InSlot = Slot;
			return true;
		}
	}

	return false;
}

void UEquipmentHolderComponent::OnRep_EquipmentSlots(FEquipmentSlots& OldEquipmentSlots)
{
	TArray<FName> SlotIds;

	for (const FEquipmentSlot& Slot : OldEquipmentSlots.Slots)
	{
		SlotIds.AddUnique(Slot.SlotId);
	}

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		SlotIds.AddUnique(Slot.SlotId);
	}

	for (FName SlotId : SlotIds)
	{
		FEquipmentSlot* OldSlot = OldEquipmentSlots.GetSlotMaybeNull(SlotId);
		FEquipmentSlot* NewSlot = EquipmentSlots.GetSlotMaybeNull(SlotId);

		PerformEquipmentSlotDeltas(OldSlot, NewSlot);
	}
}

void UEquipmentHolderComponent::PerformEquipmentSlotDeltas(FEquipmentSlot* OldSlot, FEquipmentSlot* NewSlot)
{
	// If the new slot has a non-replicated actor class then we need to spawn it before doing the deltas
	if (NewSlot && NewSlot->DefaultEquipmentClass != nullptr && !IsClassReplicated(NewSlot->DefaultEquipmentClass) && NewSlot->Equipment == nullptr)
	{
		if (OldSlot && OldSlot->Equipment && OldSlot->Equipment->GetClass() == NewSlot->DefaultEquipmentClass)
		{
			// We can carry over the old slot equipment if the class hasn't changed
			NewSlot->Equipment = OldSlot->Equipment;
		}

		// If we didn't carry over the old slot equipment then we gotta spawn it client side
		if (NewSlot->Equipment == nullptr)
		{
			NewSlot->Equipment = SpawnEquipmentClass(*NewSlot, NewSlot->DefaultEquipmentClass);
		}
	}

	AActor* LastActor = OldSlot ? OldSlot->Equipment : nullptr;
	AActor* NewActor = NewSlot ? NewSlot->Equipment : nullptr;

	if (LastActor == NewActor)
	{
		return;
	}
	else
	{
		if (LastActor)
		{
			PerformUnequip(*OldSlot, LastActor, true);
		}

		if (NewActor)
		{
			PerformEquip(*NewSlot, LastActor, NewActor);
		}
	}
}

void UEquipmentHolderComponent::PerformUnequip(FEquipmentSlot& Slot, AActor* OldEquipment, bool bDestroyOldEquipment)
{
	if (OldEquipment)
	{
		Slot.OnSlotEquipped.Broadcast(this, OldEquipment, nullptr);

		const bool bIsEquipmentPendingKillOrUnreachable = !IsValidChecked(OldEquipment) || OldEquipment->IsUnreachable();
		if (!GExitPurge && !bIsEquipmentPendingKillOrUnreachable)
		{
			if (UEquipableItemComponent* EIC = Cast<UEquipableItemComponent>(OldEquipment->GetComponentByClass(UEquipableItemComponent::StaticClass())))
			{
				if (EIC->EquipmentHolderComponent == this)
				{
					EIC->OnUnEquip();
					EIC->EquipmentHolderComponent = nullptr;
				}
			}

			if (bDestroyOldEquipment)
			{
				// Don't destroy this actor if its replicated and we don't have auth
				if (!GetOwner()->HasAuthority() && OldEquipment->GetIsReplicated())
				{
					return;
				}

				OldEquipment->Destroy();
				if (Slot.Equipment == OldEquipment)
				{
					Slot.Equipment = nullptr;
				}
			}
		}
	}
}

void UEquipmentHolderComponent::PerformEquip(FEquipmentSlot& Slot, AActor* OldEquipment, AActor* NewEquipment)
{
	if (NewEquipment)
	{
		NewEquipment->SetOwner(GetOwner());
		NewEquipment->SetInstigator(GetOwner()->GetInstigator());
		if (Slot.GetAttachComponent(GetOwner()))
		{
			NewEquipment->AttachToComponent(Slot.GetAttachComponent(GetOwner()), FAttachmentTransformRules::SnapToTargetIncludingScale, Slot.AttachBone);
			NewEquipment->SetActorRelativeTransform(Slot.AttachTransform);
		}

		NewEquipment->OnDestroyed.RemoveAll(this);
		NewEquipment->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnChildEquipmentDestroyed);

		if (UEquipableItemComponent* EIC = Cast<UEquipableItemComponent>(NewEquipment->GetComponentByClass(UEquipableItemComponent::StaticClass())))
		{
			EIC->EquipmentHolderComponent = this;
			EIC->OnEquip(OldEquipment);
		}

		Slot.OnSlotEquipped.Broadcast(this, OldEquipment, NewEquipment);
	}
}

void UEquipmentHolderComponent::OnChildEquipmentDestroyed(AActor* DestroyedActor)
{
	if (GExitPurge)
	{
		return;
	}

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment == DestroyedActor)
		{
			SetSlotEquipment(Slot, (AActor*)nullptr, false);
			return;
		}
	}
}

void UEquipmentHolderComponent::RefreshEquipment()
{
	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		SetSlotEquipment(Slot, GetDefaultEquipmentClass(Slot), true, false);
		PerformEquip(Slot, nullptr, Slot.Equipment);
	}
}

TSubclassOf<AActor> UEquipmentHolderComponent::GetDefaultEquipmentClass(const FEquipmentSlot& Slot)
{
	return Slot.DefaultEquipmentClass;
}

AActor* UEquipmentHolderComponent::SpawnEquipmentClass(const FEquipmentSlot& Slot, TSubclassOf<AActor> EquipmentClass)
{
	if (EquipmentClass == nullptr)
	{
		return nullptr;
	}

	const bool ShouldSpawnEquipment = GetOwner()->HasAuthority() || !IsClassReplicated(EquipmentClass);
	if (ShouldSpawnEquipment && GetWorld())
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.bAllowDuringConstructionScript = true;
		SpawnInfo.Owner = GetOwner();

#if WITH_EDITOR
		//SpawnInfo.bCreateActorPackage = false;
		//SpawnInfo.OverridePackage = (GetOwner() ? GetOwner()->GetExternalPackage() : nullptr);
		//Params.OverrideActorGuid = CachedInstanceData ? CachedInstanceData->ChildActorGUID : FGuid();
#endif

		//SpawnInfo.ObjectFlags |= (RF_TextExportTransient | RF_NonPIEDuplicateTransient);
		//if (bEquipmentActorsAreTransient || HasAllFlags(RF_Transient) || (GetOwner() && GetOwner()->HasAllFlags(RF_Transient)))
		//{
		//	// If this component or its owner are transient, set our created actor to transient. 
		//	SpawnInfo.ObjectFlags |= RF_Transient;
		//}

		FTransform SpawnTransform = GetOwner() ? GetOwner()->GetTransform() : FTransform::Identity;
		if (Slot.GetAttachComponent(GetOwner()))
		{
			SpawnTransform.SetLocation(Slot.GetAttachComponent(GetOwner())->GetSocketLocation(Slot.AttachBone));
			SpawnTransform.SetRotation(Slot.GetAttachComponent(GetOwner())->GetSocketRotation(Slot.AttachBone).Quaternion());
		}
		return GetWorld()->SpawnActor(EquipmentClass, &SpawnTransform, SpawnInfo);
	}
	return nullptr;
}

void UEquipmentHolderComponent::PostEditImport()
{
	Super::PostEditImport();

	return;

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		Slot.Equipment = nullptr;
		Slot.OnSlotEquipped.Clear();
	}

	RefreshEquipment();
}

FEquipmentSlot& FEquipmentSlots::CreateSlot(FName SlotId)
{
	FEquipmentSlot& OutSlot = Slots.Emplace_GetRef();
	OutSlot.SlotId = SlotId;
	MarkSlotDirty(OutSlot);
	return OutSlot;
}

FEquipmentSlot& FEquipmentSlots::GetSlot(FName SlotId)
{
	for (FEquipmentSlot& Slot : Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return Slot;
		}
	}

	checkNoEntry();
	return Slots[0];
}

const FEquipmentSlot& FEquipmentSlots::GetSlot(FName SlotId) const
{
	for (const FEquipmentSlot& Slot : Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return Slot;
		}
	}

	checkNoEntry();
	return Slots[0];
}

FEquipmentSlot* FEquipmentSlots::GetSlotMaybeNull(FName SlotId)
{
	for (FEquipmentSlot& Slot : Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return &Slot;
		}
	}

	return nullptr;
}

const FEquipmentSlot* FEquipmentSlots::GetSlotMaybeNull(FName SlotId) const
{
	for (const FEquipmentSlot& Slot : Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return &Slot;
		}
	}

	return nullptr;
}

void FEquipmentSlots::MarkSlotDirty(FEquipmentSlot& Slot)
{
	//MarkItemDirty(Slot);
}

UEquipmentHolderComponent* FEquipmentSlots::GetEquipmentHolderComponent() const
{
	check(Cast<UEquipmentHolderComponent>(OwningObject));

	return (UEquipmentHolderComponent*)OwningObject;
}

TSubclassOf<AActor> FEquipmentSlot::GetSlotClass() const
{
	TSubclassOf<AActor> CurrentEquipmentClass = Equipment ? Equipment->GetClass() : nullptr;

	//if (CurrentEquipmentClass == nullptr)
	//{
	//	CurrentEquipmentClass = DefaultEquipmentClass;
	//}
	return CurrentEquipmentClass;
}

USceneComponent* FEquipmentSlot::GetAttachComponent(AActor* OwningActor) const
{
	if (USceneComponent* OverrideComponent = Cast<USceneComponent>(AttachComponent.OverrideComponent.Get()))
	{
		return OverrideComponent;
	}

	return Cast<USceneComponent>(AttachComponent.GetComponent(OwningActor));
}

//bool FEquipmentSlot::Serialize(FArchive& Ar)
//{
//	// Use default serialization for most properties for
//	// archive configurations supported by UStruct::SerializeVersionedTaggedProperties 
//	if (Ar.IsLoading() || Ar.IsSaving() || Ar.IsCountingMemory() || Ar.IsObjectReferenceCollector())
//	{
//		StaticStruct()->SerializeTaggedProperties(Ar, (uint8*)this, StaticStruct(), nullptr);
//	}
//
//	return true;
//}

bool FEquipmentSlots::Serialize(FArchive& Ar)
{
	if (Ar.IsLoading() || Ar.IsSaving() || Ar.IsCountingMemory() || Ar.IsObjectReferenceCollector())
	{
		StaticStruct()->SerializeTaggedProperties(Ar, (uint8*)this, StaticStruct(), nullptr);
	}

	return true;
}
UE_ENABLE_OPTIMIZATION