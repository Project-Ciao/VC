// Fill out your copyright notice in the Description page of Project Settings.


#include "VCEditorSettings.h"

#include "ISettingsModule.h"
#include "Settings/VCGameUserSettings.h"

#define LOCTEXT_NAMESPACE "FVCEditor"

void FVCEditorSettings::OnModuleStartup()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Game", "VCGame_Settings",
			LOCTEXT("RuntimeSettingsName", "VC Game"), LOCTEXT("RuntimeSettingsDescription", "Settings for VC"),
			GetMutableDefault<UVCGameUserSettings>());
	}
}

void FVCEditorSettings::OnModuleShutdown()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "VCGame_Settings");
	}
}

#undef LOCTEXT_NAMESPACE