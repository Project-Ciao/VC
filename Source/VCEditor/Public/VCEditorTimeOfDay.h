// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Containers/Ticker.h"

/**
 * 
 */
class VCEDITOR_API FVCEditorTimeOfDay : public TSharedFromThis<FVCEditorTimeOfDay>
{
public:
	void OnModuleStartup();
	void OnModuleShutdown();

	virtual ~FVCEditorTimeOfDay() {}

protected:
	virtual void RegisterMenus();

	// Ticks the time of day system (if enabled)
	bool Tick(float DeltaTime);

	FTSTicker::FDelegateHandle TickDelegate;
};
