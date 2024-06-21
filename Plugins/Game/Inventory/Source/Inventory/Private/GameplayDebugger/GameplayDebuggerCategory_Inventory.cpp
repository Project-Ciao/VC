// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayDebugger/GameplayDebuggerCategory_Inventory.h"

#if WITH_GAMEPLAY_DEBUGGER

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Inventory/InventoryComponent.h"

FGameplayDebuggerCategory_Inventory::FGameplayDebuggerCategory_Inventory()
{

}

void FGameplayDebuggerCategory_Inventory::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
    if (DebugActor)
    {
        TArray<UActorComponent*> Inventories;
        DebugActor->GetComponents(UInventoryComponent::StaticClass(), Inventories);

        for (UActorComponent* Inventory : Inventories)
        {
            Cast<UInventoryComponent>(Inventory)->DescribeSelfToGameplayDebugger(this);
            AddTextLine("");
        }
    }
}

//void FGameplayDebuggerCategory_Weapon::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
//{
//   
//}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_Inventory::MakeInstance()
{
    return MakeShareable(new FGameplayDebuggerCategory_Inventory());
}

#endif // WITH_GAMEPLAY_DEBUGGER
