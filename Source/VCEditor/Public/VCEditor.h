// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "Modules/ModuleManager.h"

class FUICommandInfo;

class FVCEditor : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
	
protected:
	virtual void RegisterMenus();

	// Ticks the time of day system (if enabled)
	bool Tick(float DeltaTime);

	FTSTicker::FDelegateHandle TickDelegate;
};
