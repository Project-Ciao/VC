// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/PlayerLevelStreamingComponent.h"

#include "VC.h"
#include "VCPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Level/LevelTeleportExitInterface.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Level/DynamicLevelComponent.h"

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

	UE_LOG(LogVC, Log, TEXT("PlayerLevelStreamingComponent::StreamLevel Request. Level to load: %s, Level to unload: %s"), *TeleportLocation.Level.ToString(), *LevelToUnload.ToString());

	UE_LOG(LogVC, Warning, TEXT("World: %s"), *GetWorld()->GetName());

	if (GetWorld() == LevelToUnload.Get())
	{
		//UE_LOG(LogTemp, Fatal, TEXT("test"));
		LevelToUnload.Reset();
	}

	CurrentRequestLevelToUnload = LevelToUnload;
	CurrentRequestTeleportLocation = TeleportLocation;
	CurrentRequestPlayLevelFadeOutOnComplete = bPlayLevelFadeOutOnComplete;

	AVCPlayerController* PC = GetOuterAVCPlayerController();
	check(PC);

	PC->AddVisibleLevel(TeleportLocation.Level);
	PC->AddInvisibleLevel(LevelToUnload);

	if (TeleportLocation.DynamicLevel != nullptr)
	{
		TeleportLocation.DynamicLevel->SpawnLevel(false);
	}

	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = "OnLevelLoaded";
	Info.UUID = 1;
	Info.Linkage = 0;
	UGameplayStatics::LoadStreamLevelBySoftObjectPtr(this, TeleportLocation.Level, true, false, Info);
}

void UPlayerLevelStreamingComponent::OnLevelLoaded()
{
	if (CurrentRequestLevelToUnload.IsValid())
	{
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		Info.ExecutionFunction = "OnLevelUnloaded";
		Info.UUID = 1;
		Info.Linkage = 0;
		UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(this, CurrentRequestLevelToUnload, Info, false);
	}
	else
	{
		OnLevelUnloaded();
	}
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
	// This should already be loaded if the level is loaded
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
		UE_LOG(LogVC, Log, TEXT("Teleporting player to teleport: %s"), *Exit->GetName());

		// The exit can be marked for GC, test it here
		ensure(IsValid(Exit));

		ILevelTeleportExitInterface::Execute_TeleportToExit(Exit, PC->GetPawn());
	}
	else
	{
		UE_LOG(LogVC, Warning, TEXT("Failed to load teleport exit! %s"), *CurrentRequestTeleportLocation.Location.ToString());
	}

	Client_FinishLevelStreamingRequest();
}

void UPlayerLevelStreamingComponent::Client_FinishLevelStreamingRequest_Implementation()
{
	bIsStreamRequestInFlight = false;
	BP_OnFinishLevelStreaming(CurrentRequestPlayLevelFadeOutOnComplete);
}
