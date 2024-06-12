// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

USTRUCT(BlueprintType)
struct FTileAdjacencyMatch
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Adjacency Match")
	uint8 AdjacencyMatches;

	// If true, then it will only match on the 4 directions instead of all 8 directions
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Adjacency Match")
	bool bMatchOnCardinalOnly;

	// Offset to apply to this tile for this match
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tile Adjacency Match")
	FTransform Transform;

	bool MatchesAdjacencyArray(uint8 AdjacencyValue) const;
};

UCLASS()
class TILESYSTEM_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tile")
		TArray<FTileAdjacencyMatch> AdjacencyMatches;

	// This is called on the CDO, so don't mess with variables
	UFUNCTION(BlueprintNativeEvent, Category = "Tile")
		bool MatchesAdjacencyArray(uint8 AdjacencyValue);

	// Leaves only the north south east west bits
	UFUNCTION(BlueprintPure, Category = "Tile")
		uint8 RemoveNonCardinalBits(uint8 Value) const;

	// Removes the north south east west bits, leaving the corner bits only
	UFUNCTION(BlueprintPure, Category = "Tile")
		uint8 RemoveCardinalBits(uint8 Value) const;

	// The matched adjacency bits of THIS tile. Use for debug purposes.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Tile", meta=(AdvancedDisplay))
		uint8 AdjacencyBits;

	// The transform of this tile before it had an offset added
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Tile", meta = (AdvancedDisplay))
		FTransform OriginalTransform;

	// Called after spawning this tile with the matching adjacency bits. Sets the relative transform
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, CallInEditor, Category = "Tile", meta = (CallInEditor = true))
	void OnAdjacencyBitsSet(uint8 InAdjacencyBits);
};
