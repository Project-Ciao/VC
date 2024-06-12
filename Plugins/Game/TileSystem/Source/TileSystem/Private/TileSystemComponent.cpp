// Fill out your copyright notice in the Description page of Project Settings.


#include "TileSystemComponent.h"

#include "Tile.h"
#include "TileSystemFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

UTileSystemComponent::UTileSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	SetTickGroup(ETickingGroup::TG_LastDemotable);

	bTickInEditor = true;
}

void UTileSystemComponent::SetTile(const FIntVector& Coordinate, uint8 Value, ETileSystemOperation Operation)
{
	switch (Operation)
	{
	case ETileSystemOperation::Set:
		if (Value == 0)
		{
			Tiles.Remove(Coordinate);
		}
		else
		{
			Tiles.Add(Coordinate, Value);
		}
		MarkTileDirty(Coordinate);
		return;

	case ETileSystemOperation::Add:
		if (Value == 0)
		{
			return;
		}

		Tiles.FindOrAdd(Coordinate) |= Value;
		MarkTileDirty(Coordinate);
		return;

	case ETileSystemOperation::Remove:
		if (uint8* Tile = Tiles.Find(Coordinate))
		{
			*Tile &= (~Value);
			if (*Tile == 0)
			{
				Tiles.Remove(Coordinate);
			}
			MarkTileDirty(Coordinate);
		}
		return;

	default:
		break;
	}
}

void UTileSystemComponent::UpdateDirtyTiles()
{
	if (DirtyTiles.Num())
	{
		for (const FIntVector& Coordinate : DirtyTiles)
		{
			RefreshTile(Coordinate);
		}

		OnTileSystemUpdated.Broadcast(this, DirtyTiles);
		DirtyTiles.Empty();
	}
}

void UTileSystemComponent::RefreshAllTiles()
{
	for (TPair<FIntVector, uint8> Tile : Tiles)
	{
		DestroyAllTileActorsAtCoordinate(Tile.Key);
	}

	for (TPair<FIntVector, uint8> Tile : Tiles)
	{
		RefreshTile(Tile.Key);
	}
}

void UTileSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateDirtyTiles();
}

void UTileSystemComponent::MarkTileDirty(const FIntVector& Coordinate)
{
	DirtyTiles.AddUnique(Coordinate);
	for (int i = 0; i < 8; ++i)
	{
		FIntVector OffsetedCoordinate = Coordinate;
		OffsetedCoordinate.X += FTileSystemFunctionLibrary::AdjacentOffsetCoords[i].first;
		OffsetedCoordinate.Y += FTileSystemFunctionLibrary::AdjacentOffsetCoords[i].second;
		DirtyTiles.AddUnique(OffsetedCoordinate);
	}
}

bool UTileSystemComponent::RefreshTile(const FIntVector& Coordinate)
{
	if (uint8* Tile = Tiles.Find(Coordinate))
	{
		bool DidAnyUpdates = false;

		TArray<uint8*, TInlineAllocator<8>> AdjacencyArray = FTileSystemFunctionLibrary::LookupAdjacentTiles2D(Tiles, Coordinate);
		for (FTileActors& TileActor : TileActors)
		{
			DidAnyUpdates |= RefreshTileActors(Coordinate, AdjacencyArray, *Tile, TileActor);
		}

		return DidAnyUpdates;
	}
	else
	{
		return DestroyAllTileActorsAtCoordinate(Coordinate);
	}

	return false;
}

bool UTileSystemComponent::RefreshTileActors(const FIntVector& Coordinate, const TArray<uint8*, TInlineAllocator<8>>& AdjacencyArray, uint8 TileValue, FTileActors& TileActor)
{
	// This means the tile is supposed to be empty
	if (((TileValue & TileActor.InvisibleBit) == TileActor.InvisibleBit) || ((TileValue & TileActor.OnBit) != TileActor.OnBit))
	{
		ATile* Tile;
		if (TileActor.TileActors.RemoveAndCopyValue(Coordinate, Tile) && Tile)
		{
			Tile->Destroy();
			return true;
		}
		else
		{
			// No change, the tile was already empty
			return false;
		}
	}

	// Set up the adjacency array for testing the tile
	uint8 AdjacencyArrayBits = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (uint8* AdjacentTileValue = AdjacencyArray[i])
		{
			if ((*AdjacentTileValue & TileActor.OnBit) == TileActor.OnBit)
			{
				AdjacencyArrayBits |= (1 << i);
			}
		}
	}

	bool bChangeHappened = false;
	// If there is an existing tile at this location, check if it matches the adjacency array
	if (ATile** Tile = TileActor.TileActors.Find(Coordinate))
	{
		if (*Tile)
		{
			if ((*Tile)->MatchesAdjacencyArray(AdjacencyArrayBits))
			{
				(*Tile)->AdjacencyBits = AdjacencyArrayBits;
				//(*Tile)->RerunConstructionScripts();
				(*Tile)->OnAdjacencyBitsSet(AdjacencyArrayBits);
				return true;
			}
			else
			{
				(*Tile)->Destroy();
				TileActor.TileActors.Remove(Coordinate);
				bChangeHappened = true;
			}
		}
	}

	// If we are here, then we need to spawn the tile
	for (TSubclassOf<ATile> TileType : TileActor.TileTypes)
	{
		if (((ATile*)TileType->GetDefaultObject())->MatchesAdjacencyArray(AdjacencyArrayBits))
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(FVector(Coordinate) * TileSize);
			FActorSpawnParameters SpawnParams;
			SpawnParams.bAllowDuringConstructionScript = true;
			SpawnParams.Owner = GetOwner();
			SpawnParams.bDeferConstruction = true;

			ATile* NewTile = GetWorld()->SpawnActor<ATile>(TileType, SpawnTransform, SpawnParams);

			NewTile->AdjacencyBits = AdjacencyArrayBits;

			UGameplayStatics::FinishSpawningActor(NewTile, SpawnTransform);
			NewTile->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
			NewTile->OriginalTransform = NewTile->GetActorTransform();
			NewTile->OnAdjacencyBitsSet(AdjacencyArrayBits);

			TileActor.TileActors.Add(Coordinate, NewTile);
			return true;
		}
	}

	return bChangeHappened;
}

bool UTileSystemComponent::DestroyAllTileActorsAtCoordinate(const FIntVector& Coordinate)
{
	bool DestroyedAnyActors = false;

	for (FTileActors& TileActor : TileActors)
	{
		ATile* Tile;
		if (TileActor.TileActors.RemoveAndCopyValue(Coordinate, Tile) && Tile)
		{
			Tile->Destroy();
			DestroyedAnyActors = true;
		}
	}

	return DestroyedAnyActors;
}

#if WITH_EDITOR
void UTileSystemComponent::PostEditUndo()
{
	Super::PostEditUndo();
	OnTileSystemUpdated.Broadcast(this, {});
}
#endif

