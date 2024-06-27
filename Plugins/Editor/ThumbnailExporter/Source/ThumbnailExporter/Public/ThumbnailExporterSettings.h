// Copyright 2023 Big Cat Energising. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Containers/Map.h"
#include "ThumbnailExporterSettings.generated.h"

USTRUCT(BlueprintType)
struct FThumbnailCreationConfig
{
	GENERATED_USTRUCT_BODY()

	// If true, then the thumbnail will not be placed in the same folder as the asset, and instead will be placed in ThumbnailOverridePath
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Filename")
		bool bOverrideThumbnailPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Filename", meta = (EditCondition="bOverrideThumbnailPath", ContentDir))
		FDirectoryPath ThumbnailOverridePath;

	// If true, then the thumbnail will not use the Prefix + AssetName + Suffix for the thumbnail filename, but will instead use ThumbnailOverrideFilename
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Filename")
		bool bOverrideThumbnailFilename = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Filename", meta = (EditCondition = "bOverrideThumbnailFilename"))
		FString ThumbnailOverrideFilename;

	// Prefix added to the beginning of the generated thumbnail
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Filename", meta = (EditCondition = "!bOverrideThumbnailFilename"))
		FString ThumbnailPrefix = "T_";

	// Suffixs added to the end of the generated thumbnail
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Filename", meta = (EditCondition = "!bOverrideThumbnailFilename"))
		FString ThumbnailSuffix = "_Icon";

	// Exported thumbnail size, in pixels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Texture", meta = (ClampMin = 256, UIMin = 256))
		int32 ThumbnailSize = 256;

	// Use "SceneColor (HDR) in RGB, Inv Opacity in A" for transparency
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = Thumbnail)
		TEnumAsByte<ESceneCaptureSource> ThumbnailCaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = Thumbnail)
		TEnumAsByte<ESceneCaptureCompositeMode> ThumbnailCompositeMode = ESceneCaptureCompositeMode::SCCM_Composite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Texture")
		FLinearColor ThumbnailBackground = FLinearColor::Transparent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Texture")
		TEnumAsByte<TextureGroup> ThumbnailTextureGroup = TextureGroup::TEXTUREGROUP_UI;

	// Hide the background meshes present in the asset thumbnail. Hides the checkerboard background
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Scene")
		bool bHideThumbnailBackgroundMeshes = true;

	// Enable post-processing (like bloom) in the thumbnail
	// 
	// If transparent backgrounds are not working, try toggling off post-processing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Scene")
		bool bEnablePostProcessing = true;

	// Enable bloom in the thumbnail
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail|Scene", meta = (EditCondition = "bEnablePostProcessing"))
		bool bEnableBloom = false;

	// If true, then when the thumbnail texture is created, a notification will pop up with a link to the texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = Thumbnail)
		bool bCreateThumbnailNotification = true;

	FLinearColor GetAdjustedBackgroundColor() const
	{
		// Invert the background alpha so we can match the inverted alpha of the scene capture
		if (InvertBackgroundAlpha())
		{
			return ThumbnailBackground.CopyWithNewOpacity(1.0 - ThumbnailBackground.A);
		}
		else
		{
			return ThumbnailBackground;
		}
	}

	bool InvertBackgroundAlpha() const
	{
		return ThumbnailCaptureSource == ESceneCaptureSource::SCS_SceneColorHDR;
	}
};

USTRUCT(BlueprintType)
struct FThumbnailCreationPreset
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail Creation Preset")
		FText MenuItemName = FText::FromString("Export to Texture");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail Creation Preset")
		FText MenuItemTooltip = FText::FromString("Export an asset's thumbnail to a texture. The export preset is defined in the Thumbnail Exporter config in the project settings.");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail Creation Preset", meta = (FullyExpand=true))
		FThumbnailCreationConfig PresetConfig;
};

/**
 * 
 */
UCLASS(config = Editor, defaultconfig, DisplayName = "Thumbnail Exporter")
class THUMBNAILEXPORTER_API UThumbnailExporterSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Thumbnail Exporter Settings", meta = (TitleProperty = "{MenuItemName}", ShowOnlyInnerProperties))
		TArray<FThumbnailCreationPreset> ThumbnailCreationPresets = { FThumbnailCreationPreset() };

	static UThumbnailExporterSettings* Get() { return GetMutableDefault<UThumbnailExporterSettings>(); }
};
