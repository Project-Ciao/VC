// Fill out your copyright notice in the Description page of Project Settings.


#include "TileDebugDrawComponent.h"

#include "TileSystemFunctionLibrary.h"
#include "TileSystemComponent.h"

UTileDebugDrawComponent::UTileDebugDrawComponent()
{
	bSelectable = false;
	SetMobility(EComponentMobility::Static);
}

FDebugRenderSceneProxy* UTileDebugDrawComponent::CreateDebugSceneProxy()
{
	class FTileDebugRenderSceneProxy : public FDebugRenderSceneProxy
	{
	public:
		FTileDebugRenderSceneProxy(const UPrimitiveComponent* InComponent)
			: FDebugRenderSceneProxy(InComponent)
		{
			DrawType = EDrawType::SolidAndWireMeshes;
			ViewFlagIndex = uint32(FEngineShowFlags::FindIndexByName(TEXT("Editor")));
			ViewFlagName = TEXT("Editor");
		}

		virtual SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = IsShown(View); //&& IsSelected();
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = false;
			Result.bEditorPrimitiveRelevance = true;
			return Result;
		}
	};

	FDebugRenderSceneProxy* DebugProxy = new FTileDebugRenderSceneProxy(this);
	DrawTiles(DebugProxy);

	return DebugProxy;
}

static FString ToBinary(uint8 Bits)
{
	FString Binary;
	for (unsigned i = 128; i > 0; i = i / 2)
	{
		Binary += (Bits & i) ? "1" : "0";
	}

	return Binary;
}

void UTileDebugDrawComponent::DrawTiles(FDebugRenderSceneProxy* DebugProxy)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UTileDebugDrawComponent::DrawTiles);

	DebugProxy->Meshes.Empty();
	DebugProxy->Boxes.Empty();
	DebugProxy->Texts.Empty();
	Bounds.Min = FVector::Zero();
	Bounds.Max = FVector::Zero();

	UTileSystemComponent* TileSystem = GetTileSystem();
	if (TileSystem)
	{
		bool bHasInitializedBounds = false;
		for (const TPair<FIntVector, uint8>& Tile : TileSystem->Tiles)
		{
			if ((Tile.Value & BitMask) != BitMask)
			{
				continue;
			}

			TRACE_CPUPROFILER_EVENT_SCOPE(UTileDebugDrawComponent::DrawTiles::DrawTileSquare::DrawTile);

			// Tile world location
			FVector TileLocation;
			TileLocation.X = Tile.Key.X * TileSystem->TileSize;
			TileLocation.Y = Tile.Key.Y * TileSystem->TileSize;
			TileLocation.Z = Tile.Key.Z * TileSystem->TileSize;

			// Tile world location max
			FVector TileMax = TileLocation;
			TileMax.X += TileSystem->TileSize;
			TileMax.Y += TileSystem->TileSize;
			TileMax.Z += 4.f;

			if (!bHasInitializedBounds)
			{
				Bounds = FBox(TileLocation, TileMax);
				bHasInitializedBounds = true;
			}
			else
			{
				Bounds.Min = FVector::Min3(Bounds.Min, TileLocation, TileMax);
				Bounds.Max = FVector::Max3(Bounds.Max, TileLocation, TileMax);
			}

			FString PrintText = "";

			if (bPrintAdjacencyBits)
			{
				TArray<uint8*, TInlineAllocator<8>> AdjacencyArray = FTileSystemFunctionLibrary::LookupAdjacentTiles2D(TileSystem->Tiles, Tile.Key);
				uint8 AdjacencyBits = 0;
				for (int i = 0; i < 8; ++i)
				{
					if (uint8* AdjacentTileValue = AdjacencyArray[i])
					{
						if ((*AdjacentTileValue & BitMask) == BitMask)
						{
							AdjacencyBits |= (1 << i);
						}
					}
				}
				PrintText += FString::Printf(TEXT("%s, %d, (%d)"), *ToBinary(AdjacencyBits), AdjacencyBits, AdjacencyBits & FTileSystemFunctionLibrary::GetCardinalBits());
			}

			if (bPrintCoordinates)
			{
				PrintText += FString::Printf(TEXT("\nX: %d, Y: %d, Z: %d"), Tile.Key.X, Tile.Key.Y, Tile.Key.Z);
			}

			//if (bPrintTileDebugInfo)
			//{
			//	PrintText += "\n" + ((UTileComponent*)TileObject)->GetDebugInfo();
			//}

			if (!PrintText.IsEmpty())
			{
				DebugProxy->Texts.Emplace(PrintText, (TileLocation + TileMax) / 2.f, Color);
			}
								
			if (bDrawTiles)
			{
				//FDebugRenderSceneProxy::FMesh TileMesh;
				//TileMesh.Vertices = {
				//	FDynamicMeshVertex(FVector3f(TileLocation.X, TileLocation.Y, TileLocation.Z))
				//	, FDynamicMeshVertex(FVector3f(TileMax.X, TileLocation.Y, TileLocation.Z))
				//	, FDynamicMeshVertex(FVector3f(TileLocation.X, TileMax.Y, TileLocation.Z))
				//	, FDynamicMeshVertex(FVector3f(TileMax.X, TileMax.Y, TileLocation.Z))
				//};
				//TileMesh.Indices = { 0, 1, 2, 0, 2, 3 };
				//TileMesh.Color = Color;

				//DebugProxy->Meshes.Add(TileMesh);
				DebugProxy->Boxes.Emplace(FBox(TileLocation, TileMax), Color, FDebugRenderSceneProxy::EDrawType::WireMesh);
			}
		}
	}

	UpdateBounds();
}

void UTileDebugDrawComponent::SetTileSystem(UTileSystemComponent* NewTileSystem)
{
	TileSystemOwner = NewTileSystem ? NewTileSystem->GetTypedOuter<AActor>() : nullptr;
	if (NewTileSystem)
	{
		NewTileSystem->OnTileSystemUpdated.AddUniqueDynamic(this, &UTileDebugDrawComponent::OnTileSystemUpdated);
	}
	MarkRenderStateDirty();
}

UTileSystemComponent* UTileDebugDrawComponent::GetTileSystem() const
{
	return TileSystemOwner ? Cast<UTileSystemComponent>(TileSystemOwner->GetComponentByClass(UTileSystemComponent::StaticClass())) : nullptr;
}

void UTileDebugDrawComponent::OnTileSystemUpdated(UTileSystemComponent* TileSystemComponent, const TArray<FIntVector>& Tiles)
{
	if (TileSystemComponent == GetTileSystem())
	{
		MarkRenderStateDirty();
	}
}
