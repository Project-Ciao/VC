// Copyright 2023 Big Cat Energising. All Rights Reserved.


#include "ThumbnailExporterBlueprintFunctionLibrary.h"

#include "ThumbnailExporter.h"
#include "ThumbnailExporterSettings.h"

bool UThumbnailExporterBlueprintFunctionLibrary::ExportThumbnail(const FThumbnailCreationConfig& CreationConfig, const FAssetData& Asset, FString& ThumbnailPath, const FPreCreateThumbnail& CreationDelegate)
{
    return FModuleManager::GetModuleChecked<FThumbnailExporterModule>("ThumbnailExporter").ExportThumbnail(CreationConfig, Asset, ThumbnailPath, CreationDelegate);
}

bool UThumbnailExporterBlueprintFunctionLibrary::CanCreateThumbnail(const FAssetData& Asset)
{
    return FModuleManager::GetModuleChecked<FThumbnailExporterModule>("ThumbnailExporter").CanCreateThumbnail({Asset});
}

const TArray<FThumbnailCreationPreset>& UThumbnailExporterBlueprintFunctionLibrary::GetThumbnailExporterPresets()
{
    return UThumbnailExporterSettings::Get()->ThumbnailCreationPresets;
}

bool UThumbnailExporterBlueprintFunctionLibrary::GetThumbnailAssetPathAndFilename(const FThumbnailCreationConfig& CreationConfig, const FAssetData& Asset, FString& Path, FString& Filename)
{
    return FModuleManager::GetModuleChecked<FThumbnailExporterModule>("ThumbnailExporter").GetThumbnailAssetPathAndFilename(CreationConfig, Asset, Path, Filename);
}
