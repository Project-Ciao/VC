// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "VCSettings.generated.h"

class AInteractiveFoliageManager;

/**
 * 
 */
UCLASS(config = Game, defaultconfig)
class VC_API UVCSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UVCSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(config, EditAnywhere, Category = "VC Settings", meta=(ConfigRestartRequired = true))
	TSubclassOf<AInteractiveFoliageManager> InteractiveFoliageManagerClass;

	static const UVCSettings* Get() { return GetDefault<UVCSettings>(); };
};
