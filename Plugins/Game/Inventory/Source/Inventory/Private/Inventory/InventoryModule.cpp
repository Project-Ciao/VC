// Copyright Epic Games, Inc. All Rights Reserved.

#include "Inventory/InventoryModule.h"
#include "Modules/ModuleManager.h"
#if WITH_EDITOR
#include "ISettingsModule.h"
#endif // WITH_EDITOR
//#include "Shop/ShopSystem.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"
#include "GameplayDebugger/GameplayDebuggerCategory_Inventory.h"
#endif // WITH_GAMEPLAY_DEBUGGER

#define LOCTEXT_NAMESPACE "FInventoryModule"

void FInventoryModule::StartupModule()
{
//#if WITH_EDITOR
//    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
//    {
//        SettingsModule->RegisterSettings("Project", "Plugins", "ShopSystemSettings",
//            LOCTEXT("RuntimeSettingsName", "Shop System"),
//            LOCTEXT("RuntimeSettingsDescription", "Settings for the Shop System."),
//            GetMutableDefault<UShopSystem>()
//        );
//    }
//#endif

#if WITH_GAMEPLAY_DEBUGGER
    IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
    GameplayDebuggerModule.RegisterCategory("Inventory", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_Inventory::MakeInstance), EGameplayDebuggerCategoryState::EnabledInGameAndSimulate, 7);
    GameplayDebuggerModule.NotifyCategoriesChanged();
#endif
}

void FInventoryModule::ShutdownModule()
{
//#if WITH_EDITOR
//    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
//    {
//        SettingsModule->UnregisterSettings("Project", "Plugins", "ShopSystemSettings");
//    }
//#endif

#if WITH_GAMEPLAY_DEBUGGER
    if (IGameplayDebugger::IsAvailable())
    {
        IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
        GameplayDebuggerModule.UnregisterCategory("Inventory");
        GameplayDebuggerModule.NotifyCategoriesChanged();
    }
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FInventoryModule, Inventory)