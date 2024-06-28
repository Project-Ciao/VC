// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "VCGameUserSettings.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(FLinearColor, FGetLinearColor);

/**
 * 
 */
UCLASS(CustomFieldNotify)
class VC_API UVCGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:

	UVCGameUserSettings(const FObjectInitializer& ObjectInitializer);

	static UVCGameUserSettings* Get();

	UFUNCTION(BlueprintPure, Category = "VC|Game User Settings")
	FLinearColor GetUIBackgroundColor() const { return UIBackgroundColor; }

	UFUNCTION(BlueprintCallable, Category = "VC|Game User Settings")
	void SetUIBackgroundColor(FLinearColor NewUIBackgroundColor);

	/** A bindable delegate for background color*/
	UPROPERTY()
	FGetLinearColor UIBackgroundColorDelegate;

protected:
	UPROPERTY(config)
	FLinearColor UIBackgroundColor;

public:
	//~UGameUserSettings interface
	virtual void SetToDefaults() override;
	//virtual void LoadSettings(bool bForceReload) override;
	//virtual void ConfirmVideoMode() override;
	//virtual float GetEffectiveFrameRateLimit() override;
	//virtual void ResetToCurrentSettings() override;
	virtual void ApplyNonResolutionSettings() override;
	//virtual int32 GetOverallScalabilityLevel() const override;
	//virtual void SetOverallScalabilityLevel(int32 Value) override;
	//~End of UGameUserSettings interface
};
