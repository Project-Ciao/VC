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
	// The teleport entrance for the level. An entrance from another level will teleport the player here
	// 
	// This actor implements LevelTeleportExitInterface
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VC, meta = (DisplayThumbnail = "false", AllowedClasses = "/Script/VC.LevelTeleportExitInterface"))
	TObjectPtr<AActor> EntrancePoint;

	// The teleport exit for the level. This is the interactable exit for a level. When this is interacted with, it will teleport you to another level.
	// 
	// This actor implements LevelTeleportEntranceInterface
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VC, meta = (DisplayThumbnail = "false", AllowedClasses = "/Script/VC.LevelTeleportEntranceInterface"))
	TObjectPtr<AActor> Exit;

	/** The level of the water mesh. This is also used for placing boats properly */
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = VC)
	float WaterLevel;
};
