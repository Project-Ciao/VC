// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Level/LevelTeleportEntranceInterface.h"
#include "VCLevelFunctionLibrary.generated.h"

class UDynamicLevelComponent;

/**
 * 
 */
UCLASS()
class VC_API UVCLevelFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Teleport Location")
	static FTeleportLocation MakeTeleportLocation(TSoftObjectPtr<UWorld> Level, AActor* Location, UDynamicLevelComponent* DynamicLevel);

	UFUNCTION(BlueprintPure, Category = "Teleport Location")
	static bool IsTeleportLocationValid(const FTeleportLocation& Location);

	UFUNCTION(BlueprintPure, Category = "Teleport Location")
	static TSoftObjectPtr<UWorld> GetRemappedTeleportWorld(APlayerController* PC, const FTeleportLocation& Location);
};
