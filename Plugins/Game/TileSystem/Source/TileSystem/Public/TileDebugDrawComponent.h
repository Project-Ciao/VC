// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Debug/DebugDrawComponent.h"
#include "TileDebugDrawComponent.generated.h"

class UTileSystemComponent;

/**
 * 
 */
UCLASS(ClassGroup = ("Tile System"), Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class TILESYSTEM_API UTileDebugDrawComponent : public UDebugDrawComponent
{
	GENERATED_BODY()
public:
	UTileDebugDrawComponent();

public:
	virtual FDebugRenderSceneProxy* CreateDebugSceneProxy() override;

	void DrawTiles(FDebugRenderSceneProxy* DebugProxy);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Debug Component")
		FColor Color = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Debug Component")
		bool bDrawTiles = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Debug Component")
		bool bPrintCoordinates = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Debug Component")
		bool bPrintAdjacencyBits = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Debug Component")
		bool bPrintTileDebugInfo = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Debug Component", meta=(Bitmask))
		uint8 BitMask = 1;

	FBox Bounds;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override { return Bounds; }

	void SetTileSystem(UTileSystemComponent* NewTileSystem);
	UTileSystemComponent* GetTileSystem() const;

protected:
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tile Debug Component")
	UPROPERTY()
		AActor* TileSystemOwner;

	//UPROPERTY()
	//	UTileSystemComponent* TileSystem;

	UFUNCTION()
		void OnTileSystemUpdated(UTileSystemComponent* TileSystemComponent, const TArray<FIntVector>& Tiles);
};
