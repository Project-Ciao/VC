// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Fbx/InterchangeFbxTranslator.h"
#include "SkinnedFoliageFbxTranslator.generated.h"

/**
 * 
 */
UCLASS()
class SKINNEDFOLIAGEEDITOR_API USkinnedFoliageFbxTranslator : public UInterchangeFbxTranslator
{
	GENERATED_BODY()
	
public:
	virtual EInterchangeTranslatorAssetType GetSupportedAssetTypes() const override;
	virtual TArray<FString> GetSupportedFormats() const override;
	virtual bool Translate(UInterchangeBaseNodeContainer& BaseNodeContainer) const override;

	/**
	 * Once the translation is done, the import process need a way to retrieve payload data.
	 * This payload will be use by the factories to create the asset.
	 *
	 * @param SourceData - The source data containing the data to translate
	 * @param PayloadKey - The key to retrieve the a particular payload contain into the specified source data.
	 * @return a PayloadData containing the imported data. The TOptional will not be set if there is an error.
	 */
	virtual TFuture<TOptional<UE::Interchange::FMeshPayloadData>> GetMeshPayloadData(const FInterchangeMeshPayLoadKey& PayLoadKey, const FTransform& MeshGlobalTransform) const override;
};
