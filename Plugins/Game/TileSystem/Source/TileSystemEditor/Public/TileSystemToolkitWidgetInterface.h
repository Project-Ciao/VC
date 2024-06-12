// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TileSystemToolkitWidgetInterface.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FTileSystemToolkitWidgetInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		class UTileSystemEditorModeSettings* EditorModeSettings;

	UPROPERTY(BlueprintReadOnly)
		class UTileDebugDrawComponent* TileDrawComponent;
};

UINTERFACE(MinimalAPI)
class UTileSystemToolkitWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class TILESYSTEMEDITOR_API ITileSystemToolkitWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Tile System Toolkit Widget Interface")
		void SetToolkitInfo(const FTileSystemToolkitWidgetInfo& WidgetInfo);
};
