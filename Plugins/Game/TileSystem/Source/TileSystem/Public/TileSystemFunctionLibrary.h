// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class TILESYSTEM_API FTileSystemFunctionLibrary
{
public:
    static constexpr std::pair<int32, int32> AdjacentOffsetCoords[8] = {
        std::pair<int32, int32>(0, 1),      // 1 << 0, N
        std::pair<int32, int32>(1, 1),      // 1 << 1, NE
        std::pair<int32, int32>(1, 0),      // 1 << 2, E
        std::pair<int32, int32>(1, -1),     // 1 << 3, SE
        std::pair<int32, int32>(0, -1),     // 1 << 4, S
        std::pair<int32, int32>(-1, -1),    // 1 << 5, SW
        std::pair<int32, int32>(-1, 0),     // 1 << 6, W
        std::pair<int32, int32>(-1, 1)      // 1 << 7, NW
    };

	template<typename T>
	static TArray<T*, TInlineAllocator<8>> LookupAdjacentTiles2D(TMap<FIntVector, T>& Map, const FIntVector& Coordinates)
	{
		TArray<T*, TInlineAllocator<8>> RetVal;

        for (int i = 0; i < 8; ++i)
        {
            int32 CoordX = Coordinates.X + AdjacentOffsetCoords[i].first;
            int32 CoordY = Coordinates.Y + AdjacentOffsetCoords[i].second;
            RetVal.Push(Map.Find(FIntVector(CoordX, CoordY, Coordinates.Z)));
        }
		return RetVal;
	}

    // Bits representing the cardinal directions North, East, South, West
    static constexpr uint8 GetCardinalBits()
    {
        return 0b01010101;
    }

    // http://www.cr31.co.uk/stagecast/wang/blob.html
    template<typename T>
    static uint8 GetAdjacencyBits(TMap<FIntVector, T>& Map, const FIntVector& Coordinates, const TFunction<bool(const FIntVector& Coodinates, T* Tile)>& BitPredicate)
    {
        check(BitPredicate);

        uint8 RetVal = 0;
        for (int i = 0; i < 8; ++i)
        {
            int32 CoordX = Coordinates.X + AdjacentOffsetCoords[i].first;
            int32 CoordY = Coordinates.Y + AdjacentOffsetCoords[i].second;
            T* Tile = Map.Find(FIntVector(CoordX, CoordY, Coordinates.Z));

            if (BitPredicate(FIntVector(CoordX, CoordY, Coordinates.Z), Tile))
            {
                RetVal |= (1 << i);
            }
        }

        return RetVal;
    }
};
