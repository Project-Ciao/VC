// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkinnedFoliageEditor.h"

#include "SkinnedFoliageFbxTranslator.h"
#include "InterchangeManager.h"

#define LOCTEXT_NAMESPACE "FSkinnedFoliageEditorModule"

void FSkinnedFoliageEditorModule::StartupModule()
{
	auto RegisterItems = []()
	{
		UInterchangeManager& InterchangeManager = UInterchangeManager::GetInterchangeManager();

		//Register the translators
#if WITH_EDITOR
		InterchangeManager.RegisterTranslator(USkinnedFoliageFbxTranslator::StaticClass());
#endif
	};
	
	if (GEngine)
	{
		RegisterItems();
	}
	else
	{
		FCoreDelegates::OnPostEngineInit.AddLambda(RegisterItems);
	}
}

void FSkinnedFoliageEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSkinnedFoliageEditorModule, SkinnedFoliageEditor)