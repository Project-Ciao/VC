// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VCPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class VC_API AVCPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "VC|Player State")
	void Server_SetPlayerName(const FString& S);
};
