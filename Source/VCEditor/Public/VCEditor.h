// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FVCEditorTimeOfDay;
class FVCEditorSettings;

class FVCEditor : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;

protected:
	TSharedPtr<FVCEditorTimeOfDay> TimeOfDay;
	TSharedPtr<FVCEditorSettings> Settings;
};
