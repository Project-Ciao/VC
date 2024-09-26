// Fill out your copyright notice in the Description page of Project Settings.

#include "VCEditor.h"

#include "Logging.h"
#include "Modules/ModuleManager.h"
#include "VCEditorTimeOfDay.h"
#include "VCEditorSettings.h"

#define LOCTEXT_NAMESPACE "FVCEditor"

void FVCEditor::StartupModule()
{
	TimeOfDay = MakeShared<FVCEditorTimeOfDay>();
	TimeOfDay->OnModuleStartup();

	Settings = MakeShared<FVCEditorSettings>();
	Settings->OnModuleStartup();
}

void FVCEditor::ShutdownModule()
{
	TimeOfDay->OnModuleShutdown();
	TimeOfDay.Reset();

	Settings->OnModuleShutdown();
	Settings.Reset();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVCEditor, VCEditor);
