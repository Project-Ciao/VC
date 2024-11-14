// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InterchangePipelineBase.h"
#include "SkinnedFoliageMeshPipeline.generated.h"

/**
 * 
 */
UCLASS()
class SKINNEDFOLIAGEEDITOR_API USkinnedFoliageMeshPipeline : public UInterchangePipelineBase
{
	GENERATED_BODY()
	
public:
	/** The name of the pipeline that will be display in the import dialog. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkinnedFoliageMesh", meta = (StandAlonePipelineProperty = "True", PipelineInternalEditionData = "True"))
	FString PipelineDisplayName;

	// Creates a UV set where each U index correlates to a bone index
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkinnedFoliageMesh")
	bool bCreateFoliageBoneUVs = true;

	virtual bool CanExecuteOnAnyThread(EInterchangePipelineTask PipelineTask) override
	{
		return false;
	}

protected:
	virtual void ExecutePipeline(UInterchangeBaseNodeContainer* BaseNodeContainer, const TArray<UInterchangeSourceData*>& SourceDatas, const FString& ContentBasePath) override;
	virtual void ExecutePostFactoryPipeline(const UInterchangeBaseNodeContainer* BaseNodeContainer, const FString& NodeKey, UObject* CreatedAsset, bool bIsAReimport) override;
};
