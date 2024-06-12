// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Internationalization/Text.h"
#include "Templates/SharedPointer.h"
#include "Toolkits/BaseToolkit.h"
#include "UObject/NameTypes.h"
#include "EdMode.h"

class UTileSystemEditorModeSettings;
class UTileDebugDrawComponent;
class UTileCursorDebugDrawComponent;
class UTileSystemComponent;

class TILESYSTEMEDITOR_API FTileSystemEdMode : public FEdMode
{
public:
	FTileSystemEdMode();

	/** Destructor */
	virtual ~FTileSystemEdMode();

	/** FGCObject interface */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual bool MouseMove(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y) override;

	/**
	* FEdMode: Called when the mouse is moved while a window input capture is in effect
	*
	* @param	InViewportClient	Level editor viewport client that captured the mouse input
	* @param	InViewport			Viewport that captured the mouse input
	* @param	InMouseX			New mouse cursor X coordinate
	* @param	InMouseY			New mouse cursor Y coordinate
	*
	* @return	true if input was handled
	*/
	virtual bool CapturedMouseMove(FEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;

	virtual bool Select(AActor* InActor, bool bInSelected);

	virtual void Enter() override;

	virtual void Exit() override;

	virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;

	/** FEdMode: Called when a key is pressed */
	virtual bool InputKey(FEditorViewportClient* InViewportClient, FViewport* InViewport, FKey InKey, EInputEvent InEvent) override;

protected:
	UTileDebugDrawComponent* TileDrawComponent;
	UTileCursorDebugDrawComponent* TileCursorDrawComponent;
	UTileSystemEditorModeSettings* EditorModeSettings;

	void SetSelectedTileSystem(AActor* Actor);

	/** Trace under the mouse cursor and update tile cursor position */
	bool CursorTrace(FEditorViewportClient* ViewportClient, const FVector& InRayOrigin, const FVector& InRayDirection, FIntVector& TileHit);

	bool bIsDragging;

	UUserWidget* EdWidgetBlueprint;

	AActor* EditedActor;
	UTileSystemComponent* EditedTileSystem;

	friend class FTileSystemEdModeToolkit;
};

class TILESYSTEMEDITOR_API FTileSystemEdModeToolkit : public FModeToolkit
{
public:
	FTileSystemEdModeToolkit(FTileSystemEdMode* EdMode)
		: TileEditorMode(EdMode)
	{
	}

	/** Initializes the foliage mode toolkit */
	virtual void Init(const TSharedPtr< class IToolkitHost >& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override { return TileEditorMode; };
	virtual TSharedPtr<class SWidget> GetInlineContent() const override;

	/** Mode Toolbar Palettes **/
	virtual void GetToolPaletteNames(TArray<FName>& InPaletteName) const;
	virtual FText GetToolPaletteDisplayName(FName PaletteName) const;
	virtual void BuildToolPalette(FName PaletteName, class FToolBarBuilder& ToolbarBuilder);

	/** Modes Panel Header Information **/
	virtual FText GetActiveToolDisplayName() const;
	virtual FText GetActiveToolMessage() const;

	virtual void OnToolPaletteChanged(FName PaletteName) override;

private:
	TSharedPtr< class SWidget > EdWidget;

	FTileSystemEdMode* TileEditorMode;

};