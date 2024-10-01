// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonBorder.h"
#include "Delegates/Delegate.h"
#include "VCDefaultBorderStyle.generated.h"

class UVCGameUserSettings;

/**
 * 
 */
UCLASS()
class VCUI_API UVCDefaultBorderStyle : public UCommonBorderStyle
{
	GENERATED_BODY()
	
public:
	UVCDefaultBorderStyle();

	FSimpleMulticastDelegate OnStyleChange;

protected:
	UFUNCTION()
	void OnPostEngineInit();

	UFUNCTION()
	void OnUIBackgroundColorUpdated(UVCGameUserSettings* GameUserSettings, FLinearColor NewColor);
};
