// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TileSystemEditorSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Editor, DefaultConfig)
class TILESYSTEMEDITOR_API UTileSystemEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
		TSoftClassPtr<UUserWidget> TileSystemEditorModeToolkitWidget;

	static const UTileSystemEditorSettings* Get() { return GetDefault<UTileSystemEditorSettings>(); };
};
