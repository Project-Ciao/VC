// Fill out your copyright notice in the Description page of Project Settings.


#include "SkinnedFoliageFbxTranslator.h"

#include "MeshDescription.h"

UE_DISABLE_OPTIMIZATION
EInterchangeTranslatorAssetType USkinnedFoliageFbxTranslator::GetSupportedAssetTypes() const
{
	return EInterchangeTranslatorAssetType::Materials | EInterchangeTranslatorAssetType::Meshes;
}

TArray<FString> USkinnedFoliageFbxTranslator::GetSupportedFormats() const
{
	TArray<FString> Formats{ TEXT("fbx_sf;Filmbox (Skinned Foliage)") };
	return Formats;
}

bool USkinnedFoliageFbxTranslator::Translate(UInterchangeBaseNodeContainer& BaseNodeContainer) const
{
	return Super::Translate(BaseNodeContainer);
}

TFuture<TOptional<UE::Interchange::FMeshPayloadData>> USkinnedFoliageFbxTranslator::GetMeshPayloadData(const FInterchangeMeshPayLoadKey& PayLoadKey, const FTransform& MeshGlobalTransform) const
{
	using MeshPayloadT = TOptional<UE::Interchange::FMeshPayloadData>;

	TFuture<MeshPayloadT> Future = Super::GetMeshPayloadData(PayLoadKey, MeshGlobalTransform);
	return Future.Then([PayLoadKey](TFuture<MeshPayloadT>&& Data) -> MeshPayloadT
	{
		MeshPayloadT Payload = Data.Get();

		//Payload->MeshDescription.

		return Payload;
	});
}
UE_ENABLE_OPTIMIZATION
