// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LevelTeleportExitInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class ULevelTeleportExitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for an entrance you can interact with that will teleport the player to another level
 */
class VC_API ILevelTeleportExitInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level Streaming|Teleport Exit")
	void TeleportToExit(APawn* Pawn);
	virtual void SetTeleportLocation_Implementation(APawn* Pawn) {};
};
