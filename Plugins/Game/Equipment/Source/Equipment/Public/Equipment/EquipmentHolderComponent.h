// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/Class.h"
#include "EquipmentHolderComponent.generated.h"

struct FEquipmentSlots;
class UEquipmentHolderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSlotEquipped, UEquipmentHolderComponent*, EquipmentHolderComponent, AActor*, OldEquipment, AActor*, Equipment);

USTRUCT(BlueprintType, Blueprintable)
struct EQUIPMENT_API FEquipmentSlot
{
	GENERATED_BODY()

	FEquipmentSlot() = default;

	// The equipment that is spawned by default
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
		TSubclassOf<AActor> DefaultEquipmentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot", AdvancedDisplay)
		FName SlotId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment Slot", AdvancedDisplay)
		FComponentReference AttachComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot", AdvancedDisplay)
		FTransform AttachTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot", AdvancedDisplay)
		FName AttachBone;

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Transient, Category = "Equipment Slot")
		FOnSlotEquipped OnSlotEquipped;

	AActor* GetEquipment() const { return Equipment; }
	TSubclassOf<AActor> GetSlotClass() const;
	USceneComponent* GetAttachComponent(AActor* OwningActor = nullptr) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment Slot", AdvancedDisplay)
		AActor* Equipment;

	friend UEquipmentHolderComponent;
};

USTRUCT(BlueprintType, Blueprintable)
struct EQUIPMENT_API FEquipmentSlots
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slots", meta = (TitleProperty = "{SlotId}:{DefaultEquipmentClass}"))
		TArray<FEquipmentSlot> Slots;

	FEquipmentSlot& CreateSlot(FName SlotId);

	// WARNING: If you edit the returned slot, please mark it dirty!
	FEquipmentSlot& GetSlot(FName SlotId);

	const FEquipmentSlot& GetSlot(FName SlotId) const;

	FEquipmentSlot* GetSlotMaybeNull(FName SlotId);
	const FEquipmentSlot* GetSlotMaybeNull(FName SlotId) const;

	// Marks the slot dirty for net serialization.
	// Call this if you modify a slot or add a new slot
	void MarkSlotDirty(FEquipmentSlot& Slot);

	class UEquipmentHolderComponent* GetEquipmentHolderComponent() const;

	UPROPERTY(Transient)
		UObject* OwningObject;

	bool Serialize(FArchive& Ar);
};

template<>
struct TStructOpsTypeTraits<FEquipmentSlots> : public TStructOpsTypeTraitsBase2<FEquipmentSlots>
{
	enum
	{
		WithSerializer = true
	};
};

UCLASS( ClassGroup=(Equipment), meta=(BlueprintSpawnableComponent) )
class EQUIPMENT_API UEquipmentHolderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentHolderComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	FEquipmentSlot& GetOrCreateSlotById(FName SlotId);

	const FEquipmentSlots& GetEquipmentSlots() const { return EquipmentSlots; }

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		FEquipmentSlots& GetEquipmentSlots() { return EquipmentSlots; }

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		bool GetEquipmentSlot(FName SlotID, FEquipmentSlot& Slot);

	// Gets the component that the equipment attaches to at this slot
	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		USceneComponent* GetEquipmentSlotAttachComponent(FName SlotID) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		void SetEquipmentSlot(const FEquipmentSlot& Slot);

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		bool SetSlotEquipment(FName SlotID, TSubclassOf<AActor> Class, bool bDestroyOldEquipment = true, bool bUpdateDefaultEquipmentClass = true);

	// Returns true if the slot was found and set
	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		bool SetEquipmentSlotAttachComponent(FName SlotID, USceneComponent* AttachComponent);

	void SetSlotEquipment(FEquipmentSlot& Slot, AActor* NewEquipment, bool bDestroyOldEquipment = true, bool bUpdateDefaultEquipmentClass = true);
	void SetSlotEquipment(FEquipmentSlot& Slot, TSubclassOf<AActor> NewEquipmentClass, bool bDestroyOldEquipment = true, bool bUpdateDefaultEquipmentClass = true);

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		void RespawnSlotActor(FName SlotID);

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		bool UnequipActor(AActor* Actor, bool bDestroyActor = false);

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		bool HasEquipment(AActor* Equipment) const;

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		bool GetSlotFromEquipment(const AActor* Equipment, FEquipmentSlot& Slot);

	virtual void RefreshEquipment();

	UPROPERTY(EditDefaultsOnly, Category = "Equipment Holder")
		bool bEquipmentActorsAreTransient;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_EquipmentSlots, Category = "Equipment Holder")
		FEquipmentSlots EquipmentSlots;

	UFUNCTION()
		virtual void OnRep_EquipmentSlots(FEquipmentSlots& OldEquipmentSlots);

	void PerformEquipmentSlotDeltas(FEquipmentSlot* OldSlot, FEquipmentSlot* NewSlot);
	
	// Calls all of the associated equip/unequip functions on an actor. Does not change slots
	void PerformUnequip(FEquipmentSlot& Slot, AActor* OldEquipment, bool bDestroyOldEquipment = true);
	void PerformEquip(FEquipmentSlot& Slot, AActor* OldEquipment, AActor* NewEquipment);

	UFUNCTION()
		void OnChildEquipmentDestroyed(AActor* DestroyedActor);

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		virtual TSubclassOf<AActor> GetDefaultEquipmentClass(const FEquipmentSlot& Slot);

	// Just spawns and attaches the actor of EquipmentClass to the slot, does not call OnEquip!
	AActor* SpawnEquipmentClass(const FEquipmentSlot& Slot, TSubclassOf<AActor> EquipmentClass);
public:
	virtual void PostEditImport() override;
//#if WITH_EDITOR
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif // WITH_EDITOR
};
