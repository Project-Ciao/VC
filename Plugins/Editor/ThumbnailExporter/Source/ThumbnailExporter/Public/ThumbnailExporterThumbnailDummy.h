// Copyright 2023 Big Cat Energising. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ThumbnailExporterThumbnailDummy.generated.h"

/**
 * Dummy class used for registering the ClearBlueprintThumbnailRenderer with the engine
 */
UCLASS(Abstract, NotBlueprintable)
class THUMBNAILEXPORTER_API UThumbnailExporterThumbnailDummy : public UObject
{
	GENERATED_BODY()
	
};
