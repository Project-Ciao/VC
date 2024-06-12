// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

#include "TileSystemFunctionLibrary.h"

// Sets default values
ATile::ATile()
{

}

bool ATile::MatchesAdjacencyArray_Implementation(uint8 AdjacencyValue)
{
    for (const FTileAdjacencyMatch& AdjacencyMatch : AdjacencyMatches)
    {
        if (AdjacencyMatch.MatchesAdjacencyArray(AdjacencyValue))
        {
            return true;
        }
    }

    return false;
}

uint8 ATile::RemoveNonCardinalBits(uint8 Value) const
{
    return Value & FTileSystemFunctionLibrary::GetCardinalBits();
}

uint8 ATile::RemoveCardinalBits(uint8 Value) const
{
    return Value & ~FTileSystemFunctionLibrary::GetCardinalBits();
}

void ATile::OnAdjacencyBitsSet_Implementation(uint8 InAdjacencyBits)
{
    for (const FTileAdjacencyMatch& AdjacencyMatch : AdjacencyMatches)
    {
        if (AdjacencyMatch.MatchesAdjacencyArray(InAdjacencyBits))
        {
            SetActorRelativeTransform(OriginalTransform);
            AddActorLocalTransform(AdjacencyMatch.Transform);
            break;
        }
    }
}

bool FTileAdjacencyMatch::MatchesAdjacencyArray(uint8 AdjacencyValue) const
{
    if (bMatchOnCardinalOnly)
    {
        AdjacencyValue &= FTileSystemFunctionLibrary::GetCardinalBits();
        return (AdjacencyMatches & FTileSystemFunctionLibrary::GetCardinalBits()) == AdjacencyValue;
    }

    return AdjacencyMatches == AdjacencyValue;
}
