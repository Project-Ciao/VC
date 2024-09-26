// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class VCEDITOR_API FVCEditorSettings : public TSharedFromThis<FVCEditorSettings>
{
public:
	void OnModuleStartup();
	void OnModuleShutdown();
};
