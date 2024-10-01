// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/VCGameUserSettings.h"

UVCGameUserSettings::UVCGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetToDefaults();
}

UVCGameUserSettings* UVCGameUserSettings::Get()
{
	return GEngine ? Cast<UVCGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void UVCGameUserSettings::SetUIBackgroundColor(FLinearColor NewUIBackgroundColor)
{
	UIBackgroundColor = NewUIBackgroundColor;
	OnUIBackgroundColorUpdated.Broadcast(this, NewUIBackgroundColor);
}

void UVCGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	UIBackgroundColor = FLinearColor(0.991102f, 0.964686f, 0.822786f, 1.f);
}

void UVCGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
}

#if WITH_EDITOR
void UVCGameUserSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OnUIBackgroundColorUpdated.Broadcast(this, UIBackgroundColor);
}
#endif
