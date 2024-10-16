// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "VCWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class VC_API AVCWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	/** The level of the water mesh. This is also used for placing boats properly */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = VC)
	float WaterLevel;
};
