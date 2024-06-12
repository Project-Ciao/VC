// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TileSystemComponent.generated.h"

class ATile;

UENUM(Blueprintable, BlueprintType)
enum class ETileSystemOperation : uint8
{
	Set		UMETA(DisplayName = "Set",		ToolTip="Sets the bits in the tile to exactly the applied bitmask."),
	Add		UMETA(DisplayName = "Add",		ToolTip = "Adds the bitmask bits to the tile. Equivalent to binary OR."),
	Remove	UMETA(DisplayName = "Remove",	ToolTip = "Removes the bitmask bits from the tile.")
};

USTRUCT(Blueprintable, BlueprintType)
struct FTileActors
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Actors")
		uint8 OnBit = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Actors")
		uint8 InvisibleBit = 2;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Tile System Component")
		TMap<FIntVector, ATile*> TileActors;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Tile System Component")
		TSet<TSubclassOf<ATile>> TileTypes;
};

class UTileSystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTileSystemUpdated, UTileSystemComponent*, TileSystemComponent, const TArray<FIntVector>&, Tiles);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TILESYSTEM_API UTileSystemComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileSystemComponent();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Tile Component")
		float TileSize = 64.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Tile System Component")
		TMap<FIntVector, uint8> Tiles;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Tile System Component")
		TArray<FTileActors> TileActors;

	UFUNCTION(BlueprintCallable, Category = "Tile System Component")
		void SetTile(const FIntVector& Coordinate, uint8 Value, ETileSystemOperation Operation);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateDirtyTiles();

	UPROPERTY(BlueprintAssignable, Category = "Tile System Component")
		FOnTileSystemUpdated OnTileSystemUpdated;

	UFUNCTION(BlueprintCallable, Category = "Tile System Component", CallInEditor)
	void RefreshAllTiles();

protected:
	UPROPERTY(Transient)
		TArray<FIntVector> DirtyTiles;

	// Marks a tile and its adjacent tiles dirty
	void MarkTileDirty(const FIntVector& Coordinate);

	// Refreshes the tile actors at a coordinate. Returns true if any change occurred
	bool RefreshTile(const FIntVector& Coordinate);

	// Returns true if the TileActors changed on this tile
	bool RefreshTileActors(const FIntVector& Coordinate, const TArray<uint8*, TInlineAllocator<8>>& AdjacencyArray, uint8 TileValue, FTileActors& TileActor);

	// Returns true if an actor was succesfully destroyed at that coordinate
	bool DestroyAllTileActorsAtCoordinate(const FIntVector& Coordinate);

#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif
};
