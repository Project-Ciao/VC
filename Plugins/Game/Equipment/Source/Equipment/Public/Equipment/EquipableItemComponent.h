// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipableItemComponent.generated.h"

struct FEquipmentSlot;
class APlayerController;
class UEquipmentHolderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEquipped, UEquipableItemComponent*, EquipableItemComponent, AActor*, LastItem, bool, bFromSave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnEquipped, UEquipableItemComponent*, EquipableItemComponent, bool, bFromSave);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FText, FGetEquipmentName, UEquipableItemComponent*, EquipableItemComponent);

/**
 * Put this on an equipable item to access its attached owner's EquipmentHolderComponent and also some equip/unequip delegates
 */
UCLASS( ClassGroup=(Equipment), meta=(BlueprintSpawnableComponent) )
class EQUIPMENT_API UEquipableItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEquipableItemComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnEquip(AActor* LastItem, bool bFromSave = false);

	virtual void OnUnEquip(bool bFromSave = false);

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipable Item", meta = (DisplayName = "On Equip"))
		void BP_OnEquip(AActor* LastItem, bool bFromSave = false);

	UPROPERTY(BlueprintAssignable, Category = "Equipable Item")
		FOnEquipped OnEquippedDelegate;

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipable Item", meta = (DisplayName = "On UnEquip"))
		void BP_OnUnEquip(bool bFromSave = false);

	UPROPERTY(BlueprintAssignable, Category = "Equipable Item")
		FOnUnEquipped OnUnEquippedDelegate;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Equipable Item")
		bool CanEquip(const UEquipmentHolderComponent* InEquipmentHolderComponent) const;

	UFUNCTION(BlueprintPure, Category = "Equipable Item")
		UEquipmentHolderComponent* GetEquipmentHolderComponent() const;

	// Gets the slot this equipment is attached to. Will return false if it can't find it
	UFUNCTION(BlueprintPure, Category = "Equipable Item")
		bool GetSlot(FEquipmentSlot& Slot) const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Equipable Item")
		FText GetEquipmentName();

	// Set this to set an override for the GetEquipmentName function
	UPROPERTY(BlueprintReadWrite, Category = "Equipable Item")
		FGetEquipmentName GetEquipmentNameDelegate;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Equipable Item|Input")
		bool bBindInputEvents;

	UPROPERTY(Transient, DuplicateTransient, BlueprintReadOnly, VisibleInstanceOnly, Category = "Equipable Item")
		bool bIsEquipped;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Equipable Item")
		TWeakObjectPtr<UEquipmentHolderComponent> EquipmentHolderComponent;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void BindInputs();
	virtual void UnbindInputs();

	UFUNCTION(BlueprintPure, Category = "Equipable Item")
		virtual APawn* GetEquipmentHolderAsPawn() const;

	UFUNCTION(BlueprintPure, Category = "Equipable Item")
		virtual APlayerController* GetEquipmentHolderPC() const;
protected:

	UFUNCTION()
	virtual void OnEquipmentHolderReceivePossess(APawn* Pawn, AController* OldController, AController* NewController);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipable Item")
		FText EquipmentName;

	friend class UEquipmentHolderComponent;
};
