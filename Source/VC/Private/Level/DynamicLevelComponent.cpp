// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/DynamicLevelComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/LevelStreamingDynamic.h"

UDynamicLevelComponent::UDynamicLevelComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	LevelOptions = FDynamicLevelOptions();
	LevelStreaming = nullptr;
}

void UDynamicLevelComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDynamicLevelComponent, LevelOptions);
}

void UDynamicLevelComponent::GenerateNewLevelGUID()
{
	LevelOptions.LevelGUID = FGuid::NewGuid();
}

bool UDynamicLevelComponent::SpawnLevel(bool MulticastSpawn)
{
	if (!LevelOptions.IsValid())
	{
		return false;
	}

	bool bSuccess;
	LevelStreaming = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this, LevelOptions.Level, LevelOptions.LevelTransform, bSuccess, LevelOptions.GetDynamicLevelName(), LevelOptions.OptionalLevelStreamingClass);

	if (bSuccess && LevelStreaming && MulticastSpawn && GetOwner()->HasAuthority())
	{
		Multicast_SpawnLevel(LevelOptions);
	}

	return bSuccess && LevelStreaming;
}

void UDynamicLevelComponent::Multicast_SpawnLevel_Implementation(const FDynamicLevelOptions& InLevelOptions)
{
	// SpawnLevel handles level spawning on auth
	if (GetOwner()->HasAuthority())
	{
		return;
	}

	LevelOptions = InLevelOptions;
	
	bool bSuccess;
	LevelStreaming = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(this, LevelOptions.Level, LevelOptions.LevelTransform, bSuccess, LevelOptions.GetDynamicLevelName(), LevelOptions.OptionalLevelStreamingClass);
}

void UDynamicLevelComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool FDynamicLevelOptions::IsValid() const
{
	return LevelGUID.IsValid() && !Level.IsNull();
}

FString FDynamicLevelOptions::GetDynamicLevelName() const
{
	if (!LevelGUID.IsValid() || Level.IsNull())
	{
		return FString();
	}

	return Level.GetAssetName() + "_" + LevelGUID.ToString();
}
