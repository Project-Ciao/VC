// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Level/LevelTeleportEntranceInterface.h"
#include "PlayerLevelStreamingComponent.generated.h"


UCLASS( ClassGroup=("Level Streaming"), meta = (BlueprintSpawnableComponent), Blueprintable, Within="VCPlayerController")
class VC_API UPlayerLevelStreamingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerLevelStreamingComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Player Level Streaming Component")
	void StreamLevel(TSoftObjectPtr<UWorld> LevelToUnload, const FTeleportLocation& TeleportLocation, bool bPlayLevelFadeOutOnComplete);

protected:
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Player Level Streaming Component")
	bool bIsStreamRequestInFlight;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Player Level Streaming Component")
	FTeleportLocation CurrentRequestTeleportLocation;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Player Level Streaming Component")
	TSoftObjectPtr<UWorld> CurrentRequestLevelToUnload;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Player Level Streaming Component")
	bool CurrentRequestPlayLevelFadeOutOnComplete;

	UFUNCTION()
	void OnLevelLoaded();

	UFUNCTION()
	void OnLevelUnloaded();

	UFUNCTION(Server, Reliable)
	void Server_NotifyFinishLevelStreaming(const FTeleportLocation& TeleportLocation, bool bPlayLevelFadeOutOnComplete);

	UFUNCTION()
	void OnLevelLoadedServer();

	void OnTeleportLocationLoadedServer();

	UFUNCTION(Client, Reliable)
	void Client_FinishLevelStreamingRequest();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Level Streaming Component")
	void BP_OnFinishLevelStreaming(bool bPlayLevelFadeOutOnComplete);
};
