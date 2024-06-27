// Copyright 2023 Big Cat Energising. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ThumbnailExporterBlueprintFunctionLibrary.h"

struct FAssetData;
struct FThumbnailCreationConfig;

class FThumbnailExporterModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Exports the thumbnail to a separate texture, optionally creates a notification saying the texture was created
	// Returns true if the creation was succesful
	static bool ExportThumbnail(const FThumbnailCreationConfig& CreationConfig, const FAssetData& Asset, FString& ThumbnailPath, const FPreCreateThumbnail& CreationDelegate = {});

	// Returns true if a thumbnail can be created for the asset(s)
	static bool CanCreateThumbnail(const TArray<FAssetData>& Assets);

	// Returns the path and filename for the new thumbnail. The path and filename are generated from the creation config.
	// Returns false if it could not generate the path
	static bool GetThumbnailAssetPathAndFilename(const FThumbnailCreationConfig& CreationConfig, const FAssetData& Asset, FString& Path, FString& Filename);

protected:
	FDelegateHandle ContentBrowserExtenderDelegateHandle;

	void AddContentBrowserContextMenuExtender();
	void RemoveContentBrowserContextMenuExtender() const;

	static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets);

	static void ExecuteSaveThumbnailAsTexture(FMenuBuilder& MenuBuilder, const TArray<FAssetData> SelectedAssets);

	static void CreateThumbnailNotification(UTexture2D* NewTexture);
};
