// Copyright Epic Games, Inc. All Rights Reserved.

#include "TileSystemEditorModule.h"

#include "TileAdjacencyMatchCustomization.h"
#include "EditorModeRegistry.h"
#include "TileSystemEdMode.h"

#define LOCTEXT_NAMESPACE "FTileSystemEditorModule"

void FTileSystemEditorModule::StartupModule()
{
	FEditorModeRegistry::Get().RegisterMode<FTileSystemEdMode>(FName(TEXT("TileSystemEditorMode")), FText::FromString("Tile System Editor Mode"), FSlateIcon(), true);
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FTileSystemEditorModule::OnPostEngineInit);
}

void FTileSystemEditorModule::OnPostEngineInit()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("TileAdjacencyMatch", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTileAdjacencyMatchCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FTileSystemEditorModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FName(TEXT("TileSystemEditorMode")));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTileSystemEditorModule, TileSystemEditor)