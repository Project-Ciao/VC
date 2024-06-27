// Copyright 2023 Big Cat Energising. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ObjectTools.h"
#include "ThumbnailExporterBlueprintFunctionLibrary.h"

struct FThumbnailCreationConfig;
class FObjectThumbnail;

class THUMBNAILEXPORTER_API FThumbnailExporterRenderer
{
public:
	static FObjectThumbnail* GenerateThumbnail(FThumbnailCreationConfig& CreationConfig, UObject* InObject, const FPreCreateThumbnail& CreationDelegate = {});
	static void RenderThumbnail(FThumbnailCreationConfig& CreationConfig, UObject* InObject, const uint32 InImageWidth, const uint32 InImageHeight, ThumbnailTools::EThumbnailTextureFlushMode::Type InFlushMode, FObjectThumbnail* OutThumbnail = NULL, const FPreCreateThumbnail& CreationDelegate = {});
};
