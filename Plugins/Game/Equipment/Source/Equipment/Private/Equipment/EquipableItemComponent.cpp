// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipableItemComponent.h"
#include "Equipment/EquipmentHolderComponent.h"

#include "GameFramework/InputSettings.h"

UEquipableItemComponent::UEquipableItemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bBindInputEvents = true;
	bIsEquipped = false;
	EquipmentHolderComponent = nullptr;
}

void UEquipableItemComponent::OnEquip(AActor* LastItem, bool bFromSave)
{
	if (!bIsEquipped && EquipmentHolderComponent.IsValid())
	{
		bIsEquipped = true;

		if (GetOwner() && EquipmentHolderComponent->GetOwner())
		{
			GetOwner()->SetInstigator(EquipmentHolderComponent->GetOwner()->GetInstigator());

			if (GetOwner()->HasAuthority())
			{
				GetOwner()->SetAutonomousProxy(true);
			}

			if (bBindInputEvents)
			{
				BindInputs();
			}
		}

		BP_OnEquip(LastItem, bFromSave);
		OnEquippedDelegate.Broadcast(this, LastItem, bFromSave);
	}
}

void UEquipableItemComponent::OnUnEquip(bool bFromSave)
{
	if (bIsEquipped)
	{
		bIsEquipped = false;

		if (GetOwner())
		{
			GetOwner()->SetInstigator(nullptr);

			UnbindInputs();
		}

		BP_OnUnEquip(bFromSave);
		OnUnEquippedDelegate.Broadcast(this, bFromSave);
	}
}

FText UEquipableItemComponent::GetEquipmentName_Implementation()
{
	if (GetEquipmentNameDelegate.IsBound())
	{
		return GetEquipmentNameDelegate.Execute(this);
	}

	return EquipmentName;
}

bool UEquipableItemComponent::CanEquip_Implementation(const UEquipmentHolderComponent* InEquipmentHolderComponent) const
{
	return true;
}

UEquipmentHolderComponent* UEquipableItemComponent::GetEquipmentHolderComponent() const
{
	return EquipmentHolderComponent.Get();
}

bool UEquipableItemComponent::GetSlot(FEquipmentSlot& Slot) const
{
	if (UEquipmentHolderComponent* Holder = GetEquipmentHolderComponent())
	{
		return Holder->GetSlotFromEquipment(GetOwner(), Slot);
	}
	return false;
}

void UEquipableItemComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	if (EquipmentHolderComponent.IsValid())
	{
		if (!EquipmentHolderComponent->HasEquipment(GetOwner()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Equipment has no holder component! %s"), *GetOwner()->GetName());

			// Do an async task because if we delete here it crashes the editor
			AsyncTask(ENamedThreads::GameThread, [Owner = TWeakObjectPtr<AActor>(GetOwner())]()
			{
				if (Owner.IsValid())
				{
					Owner->Destroy();
				}
			});
		}
	}
#endif
}

void UEquipableItemComponent::OnUnregister()
{
	if (!IsUnreachable())
	{
		OnUnEquip();
	}

	Super::OnUnregister();
}

void UEquipableItemComponent::BindInputs()
{
	if (APlayerController* PC = GetEquipmentHolderPC())
	{
		if (PC->IsLocalController())
		{
			GetOwner()->EnableInput(PC);
		}
	}
	
	if (APawn* Pawn = GetEquipmentHolderAsPawn())
	{
		Pawn->ReceiveControllerChangedDelegate.AddUniqueDynamic(this, &ThisClass::OnEquipmentHolderReceivePossess);
	}
}

void UEquipableItemComponent::UnbindInputs()
{
	if (APawn* Pawn = GetEquipmentHolderAsPawn())
	{
		Pawn->ReceiveControllerChangedDelegate.RemoveAll(this);
	}

	if (APlayerController* PC = GetEquipmentHolderPC())
	{
		if (PC->IsLocalController())
		{
			GetOwner()->DisableInput(PC);
		}
	}
}

APawn* UEquipableItemComponent::GetEquipmentHolderAsPawn() const
{
	if (!EquipmentHolderComponent.IsValid())
	{
		return nullptr;
	}

	if (EquipmentHolderComponent->GetOwner() == nullptr)
	{
		return nullptr;
	}

	return Cast<APawn>(EquipmentHolderComponent->GetOwner());
}

APlayerController* UEquipableItemComponent::GetEquipmentHolderPC() const
{
	if (APawn* Pawn = GetEquipmentHolderAsPawn())
	{
		return Cast<APlayerController>(Pawn->GetController());
	}

	return nullptr;
}

void UEquipableItemComponent::OnEquipmentHolderReceivePossess(APawn* Pawn, AController* OldController, AController* NewController)
{
	if (Pawn != GetEquipmentHolderAsPawn())
	{
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(OldController))
	{
		GetOwner()->DisableInput(PC);
	}

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		if (PC->IsLocalController())
		{
			GetOwner()->EnableInput(PC);
		}
	}
}
