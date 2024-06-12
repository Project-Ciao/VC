// Fill out your copyright notice in the Description page of Project Settings.


#include "TileCursorDebugDrawComponent.h"

#include "TileDebugDrawComponent.h"
#include "TileSystemComponent.h"

UTileCursorDebugDrawComponent::UTileCursorDebugDrawComponent()
{
	bSelectable = false;
}

FDebugRenderSceneProxy* UTileCursorDebugDrawComponent::CreateDebugSceneProxy()
{
	class FTileCursorDebugRenderSceneProxy : public FDebugRenderSceneProxy
	{
	public:
		FTileCursorDebugRenderSceneProxy(const UPrimitiveComponent* InComponent)
			: FDebugRenderSceneProxy(InComponent)
		{
			SolidMeshMaterial = GEngine->DebugMeshMaterial;
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
			Result.bDrawRelevance = IsShown(View);
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = false;
			Result.bEditorPrimitiveRelevance = true;
			Result.bSeparateTranslucency = Result.bNormalTranslucency = IsShown(View);
			return Result;
		}
	};

	FDebugRenderSceneProxy* DebugProxy = new FTileCursorDebugRenderSceneProxy(this);

	Update(DebugProxy);

	return DebugProxy;
}

void UTileCursorDebugDrawComponent::Update(FDebugRenderSceneProxy* DebugProxy)
{
	DebugProxy->Meshes.Empty();
	DebugProxy->Boxes.Empty();
	DebugProxy->Texts.Empty();
	Bounds.Min = FVector::Zero();
	Bounds.Max = FVector::Zero();

	bool bHasInitializedBounds = false;
	if (TileDrawComponent && TileDrawComponent->GetTileSystem())
	{
		FIntVector MinValue;
		MinValue.X = FMath::Min(TileSelection.Key.X, TileSelection.Value.X);
		MinValue.Y = FMath::Min(TileSelection.Key.Y, TileSelection.Value.Y);
		MinValue.Z = FMath::Min(TileSelection.Key.Z, TileSelection.Value.Z);

		FIntVector MaxValue;
		MaxValue.X = FMath::Max(TileSelection.Key.X, TileSelection.Value.X);
		MaxValue.Y = FMath::Max(TileSelection.Key.Y, TileSelection.Value.Y);
		MaxValue.Z = FMath::Max(TileSelection.Key.Z, TileSelection.Value.Z);

		// Tile world location
		FVector TileLocation = FVector(MinValue) * TileDrawComponent->GetTileSystem()->TileSize;

		// Tile world location max
		FVector TileMax = FVector(MaxValue + FIntVector(1,1,0)) * TileDrawComponent->GetTileSystem()->TileSize;
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

		FDebugRenderSceneProxy::FMesh& TileMesh = DebugProxy->Meshes.AddDefaulted_GetRef();
		TileMesh.Vertices = {
			FDynamicMeshVertex(FVector3f(TileLocation.X, TileLocation.Y, TileLocation.Z))
			, FDynamicMeshVertex(FVector3f(TileMax.X, TileLocation.Y, TileLocation.Z))
			, FDynamicMeshVertex(FVector3f(TileLocation.X, TileMax.Y, TileLocation.Z))
			, FDynamicMeshVertex(FVector3f(TileMax.X, TileMax.Y, TileLocation.Z))
		};
		TileMesh.Indices = { 0, 1, 2, 0, 2, 3 };
		TileMesh.Color = TileDrawComponent->Color;

		DebugProxy->Boxes.Emplace(FBox(TileLocation, TileMax), TileDrawComponent->Color);
	}

	UpdateBounds();
}

void UTileCursorDebugDrawComponent::SetTileDrawComponent(UTileDebugDrawComponent* NewTileDrawComponent)
{
	TileDrawComponent = NewTileDrawComponent;
	MarkRenderStateDirty();
}

void UTileCursorDebugDrawComponent::SetTileSelection(const TPair<FIntVector, FIntVector>& NewTileSelection)
{
	if (NewTileSelection != TileSelection)
	{
		TileSelection = NewTileSelection;
		MarkRenderStateDirty();
	}
}
