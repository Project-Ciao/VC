// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LevelTeleportEntranceInterface.generated.h"

class ULevelTeleportExitInterface;
class UDynamicLevelComponent;

USTRUCT(BlueprintType)
struct FTeleportLocation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Teleport Location")
	TSoftObjectPtr<UWorld> Level;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Teleport Location")
	TSoftObjectPtr<ULevelTeleportExitInterface> Location;

	// Optional dynamic level component ptr. If this level isn't loaded it will be loaded
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Teleport Location")
	UDynamicLevelComponent* DynamicLevel;
};

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class ULevelTeleportEntranceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for a teleport exit location for a level entrance
 */
class VC_API ILevelTeleportEntranceInterface
{
	GENERATED_BODY()

public:
	// Set the location that this entrace teleports to.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level Streaming|Teleport Entrance")
	void SetTeleportLocation(const FTeleportLocation& TeleportLocation);
	virtual void SetTeleportLocation_Implementation(const FTeleportLocation& TeleportLocation) {};
};
