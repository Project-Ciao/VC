// Fill out your copyright notice in the Description page of Project Settings.


#include "VCDefaultBorderStyle.h"

#include "Settings/VCGameUserSettings.h"

UVCDefaultBorderStyle::UVCDefaultBorderStyle()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FCoreDelegates::OnPostEngineInit.AddUObject(this, &ThisClass::OnPostEngineInit);
	}
}

void UVCDefaultBorderStyle::OnPostEngineInit()
{
	if (ensure(UVCGameUserSettings::Get()))
	{
		Background.DrawAs = ESlateBrushDrawType::RoundedBox;
		Background.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		Background.OutlineSettings.CornerRadii.Set(64.0, 64.0, 64.0, 64.0);
		Background.TintColor = FSlateColor(UVCGameUserSettings::Get()->GetUIBackgroundColor());

		UVCGameUserSettings::Get()->OnUIBackgroundColorUpdated.AddUniqueDynamic(this, &ThisClass::OnUIBackgroundColorUpdated);
	}
}

void UVCDefaultBorderStyle::OnUIBackgroundColorUpdated(UVCGameUserSettings* GameUserSettings, FLinearColor NewColor)
{
	Background.DrawAs = ESlateBrushDrawType::RoundedBox;
	Background.OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	Background.OutlineSettings.CornerRadii.Set(64.0, 64.0, 64.0, 64.0);
	Background.TintColor = FSlateColor(NewColor);
}
