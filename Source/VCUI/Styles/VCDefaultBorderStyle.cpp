// Fill out your copyright notice in the Description page of Project Settings.


#include "VCDefaultBorderStyle.h"

#include "Settings/VCGameUserSettings.h"

UVCDefaultBorderStyle::UVCDefaultBorderStyle()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		//UVCGameUserSettings::Get()->ApplyResolutionSettings
		//Background.TintColor = FSlateColor(UVCGameUserSettings::Get()->GetUIBackgroundColor());
	}
}
