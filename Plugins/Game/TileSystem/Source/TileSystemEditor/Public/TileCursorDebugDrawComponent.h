// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Debug/DebugDrawComponent.h"
#include "TileCursorDebugDrawComponent.generated.h"

class UTileDebugDrawComponent;

/**
 * 
 */
UCLASS()
class TILESYSTEMEDITOR_API UTileCursorDebugDrawComponent : public UDebugDrawComponent
{
	GENERATED_BODY()
	
public:
	UTileCursorDebugDrawComponent();

	virtual FDebugRenderSceneProxy* CreateDebugSceneProxy() override;

	void SetTileDrawComponent(UTileDebugDrawComponent* NewTileDrawComponent);
	void SetTileSelection(const TPair<FIntVector, FIntVector>& NewTileSelection);
	TPair<FIntVector, FIntVector> GetTileSelection() const { return TileSelection; };

protected:
	UPROPERTY()
		UTileDebugDrawComponent* TileDrawComponent;

	TPair<FIntVector, FIntVector> TileSelection;
	FBox Bounds;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override { return Bounds; }
	void Update(FDebugRenderSceneProxy* DebugProxy);
};
