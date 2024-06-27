// Copyright 2023 Big Cat Energising. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ThumbnailExporterSettings.h"
#include "ThumbnailExporterBlueprintFunctionLibrary.generated.h"

struct FThumbnailCreationPreset;
struct FThumbnailCreationConfig;

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FThumbnailCreationConfig, FPreCreateThumbnail, const struct FThumbnailCreationConfig&, CreationConfig, AActor*, ThumbnailActor);

/**
 * 
 */
UCLASS()
class THUMBNAILEXPORTER_API UThumbnailExporterBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	// Exports the thumbnail to a separate texture, optionally creates a notification saying the texture was created
	// Returns true if the creation was succesful
	UFUNCTION(BlueprintCallable, Category = "Thumbnail Exporter", meta=(AutoCreateRefTerm="CreationDelegate"))
		static bool ExportThumbnail(const FThumbnailCreationConfig& CreationConfig, const FAssetData& Asset, FString& ThumbnailPath, const FPreCreateThumbnail& CreationDelegate);
	
	// Returns true if a thumbnail can be created for the asset
	UFUNCTION(BlueprintPure, Category = "Thumbnail Exporter")
		static bool CanCreateThumbnail(const FAssetData& Asset);

	// Gets the thumbnail creation presets defined in the Thumbnail Exporter settings.
	// The Thumbnail Exporter settings are defined inside the project settings, under the Editor section.
	UFUNCTION(BlueprintPure, Category = "Thumbnail Exporter")
		static const TArray<FThumbnailCreationPreset>& GetThumbnailExporterPresets();

	// Returns the path and filename for the new thumbnail. The path and filename are generated from the creation config.
	// Returns false if it could not generate the path
	UFUNCTION(BlueprintPure, Category = "Thumbnail Exporter")
		static bool GetThumbnailAssetPathAndFilename(const FThumbnailCreationConfig& CreationConfig, const FAssetData& Asset, FString& Path, FString& Filename);
};
