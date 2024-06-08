// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FindSessionsCallbackProxy.h"
#include "VCSteamFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VCSTEAM_API UVCSteamFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Online")
	static FBlueprintSessionResult SteamSessionIDToSessionResult(const FString& ID);
};
