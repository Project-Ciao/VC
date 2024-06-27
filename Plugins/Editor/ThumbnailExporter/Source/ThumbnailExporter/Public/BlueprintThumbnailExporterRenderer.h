// Copyright 2023 Big Cat Energising. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThumbnailRendering/BlueprintThumbnailRenderer.h"
#include "ThumbnailExporter.h"
#include "BlueprintThumbnailExporterRenderer.generated.h"

struct FThumbnailCreationConfig;
class FThumbnailExporterScene;

struct FThumbnailCreationParams
{
	FThumbnailCreationParams(FThumbnailCreationConfig& InCreationConfig)
		: CreationConfig(InCreationConfig)
	{

	}

	FThumbnailCreationConfig& CreationConfig;
	UObject* Object;
	uint32 Width;
	uint32 Height;
	FRenderTarget* RenderTarget; // 
	FCanvas* Canvas;
	bool bAdditionalViewFamily;
	bool bIsAlpha; // If true, then we are rendering out the alpha 
	FPreCreateThumbnail CreationDelegate;

	FVector2D GetThumbnailSize() const
	{
		return FVector2D(Width, Height);
	}
};

UCLASS()
class THUMBNAILEXPORTER_API UBlueprintThumbnailExporterRenderer : public UBlueprintThumbnailRenderer
{
	GENERATED_BODY()
	
public:
	UBlueprintThumbnailExporterRenderer(const FObjectInitializer& ObjectInitializer);
	virtual ~UBlueprintThumbnailExporterRenderer();

	virtual void DrawThumbnailWithConfig(FThumbnailCreationParams& CreationParams);
	virtual bool CanVisualizeAsset(UObject* Object) override;

	virtual void BeginDestroy() override;

	static void RenderViewFamily(FCanvas* Canvas, class FSceneViewFamily* ViewFamily, class FSceneView* View);

#if ENGINE_MINOR_VERSION >= 4
	virtual EThumbnailRenderFrequency GetThumbnailRenderFrequency(UObject* Object) const override { return EThumbnailRenderFrequency::Once; }
#endif

protected:
	FThumbnailExporterScene& GetThumbnailScene(const FThumbnailCreationConfig& CreationConfig);
	TArray<FThumbnailExporterScene*> ThumbnailScenes;
};
