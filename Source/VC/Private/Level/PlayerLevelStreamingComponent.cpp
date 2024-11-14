// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/PlayerLevelStreamingComponent.h"

#include "VCPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Level/LevelTeleportExitInterface.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

UPlayerLevelStreamingComponent::UPlayerLevelStreamingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	bIsStreamRequestInFlight = false;
}

void UPlayerLevelStreamingComponent::StreamLevel(TSoftObjectPtr<UWorld> LevelToUnload, const FTeleportLocation& TeleportLocation, bool bPlayLevelFadeOutOnComplete)
{
	if (bIsStreamRequestInFlight)
	{
		return;
	}
	bIsStreamRequestInFlight = true;

	CurrentRequestLevelToUnload = LevelToUnload;
	CurrentRequestTeleportLocation = TeleportLocation;
	CurrentRequestPlayLevelFadeOutOnComplete = bPlayLevelFadeOutOnComplete;

	AVCPlayerController* PC = GetOuterAVCPlayerController();
	check(PC);

	PC->AddVisibleLevel(TeleportLocation.Level);
	PC->AddInvisibleLevel(LevelToUnload);

	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = "OnLevelLoaded";
	Info.UUID = 1;
	Info.Linkage = 0;
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this, TeleportLocation.Level, true, false, Info);
}

void UPlayerLevelStreamingComponent::OnLevelLoaded()
{
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = "OnLevelUnloaded";
	Info.UUID = 1;
	Info.Linkage = 0;
	UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(this, CurrentRequestLevelToUnload, Info, false);
}

void UPlayerLevelStreamingComponent::OnLevelUnloaded()
{
	Server_NotifyFinishLevelStreaming(CurrentRequestTeleportLocation, CurrentRequestPlayLevelFadeOutOnComplete);
}

void UPlayerLevelStreamingComponent::Server_NotifyFinishLevelStreaming_Implementation(const FTeleportLocation& TeleportLocation, bool bPlayLevelFadeOutOnComplete)
{
	CurrentRequestTeleportLocation = TeleportLocation;
		 
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = "OnLevelLoadedServer";
	Info.UUID = 1;
	Info.Linkage = 0;
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this, TeleportLocation.Level, true, false, Info);
}

void UPlayerLevelStreamingComponent::OnLevelLoadedServer()
{
	// This should already be loaded
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(CurrentRequestTeleportLocation.Location.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::OnTeleportLocationLoadedServer));
}

void UPlayerLevelStreamingComponent::OnTeleportLocationLoadedServer()
{
	AVCPlayerController* PC = GetOuterAVCPlayerController();
	check(PC);

	if (PC->GetPawn() == nullptr)
	{
		return;
	}

	// CurrentRequestTeleportLocation.Location.Get() will try to cast to ULevelTeleportExitInterface, which will always fail
	// So we use ResolveObject instead
	UObject* Exit = CurrentRequestTeleportLocation.Location.GetUniqueID().ResolveObject();
	if (ensure(Exit != nullptr))
	{
		ILevelTeleportExitInterface::Execute_TeleportToExit(Exit, PC->GetPawn());
	}

	Client_FinishLevelStreamingRequest();
}

void UPlayerLevelStreamingComponent::Client_FinishLevelStreamingRequest_Implementation()
{
	bIsStreamRequestInFlight = false;
	BP_OnFinishLevelStreaming(CurrentRequestPlayLevelFadeOutOnComplete);
}
