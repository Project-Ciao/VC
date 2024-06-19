// Fill out your copyright notice in the Description page of Project Settings.


#include "VCPlayerController.h"

#include "GameFramework/PlayerState.h"
#include "Engine/WorldComposition.h"
#include "LevelUtils.h"
#include "Net/UnrealNetwork.h"
#include "Engine/LevelStreamingVolume.h"

static const TCHAR* A(bool b)
{
	return b ? TEXT("true") : TEXT("false");
}

/**
 * Streaming settings for levels which are determined visible by level streaming volumes.
 */
class FVisibleLevelStreamingSettings
{
public:
	FVisibleLevelStreamingSettings()
	{
		bShouldBeVisible = false;
		bShouldBlockOnLoad = false;
		bShouldChangeVisibility = false;
	}

	FVisibleLevelStreamingSettings(EStreamingVolumeUsage Usage)
	{
		switch (Usage)
		{
		case SVB_Loading:
			bShouldBeVisible = false;
			bShouldBlockOnLoad = false;
			bShouldChangeVisibility = false;
			break;
		case SVB_LoadingNotVisible:
			bShouldBeVisible = false;
			bShouldBlockOnLoad = false;
			bShouldChangeVisibility = true;
			break;
		case SVB_LoadingAndVisibility:
			bShouldBeVisible = true;
			bShouldBlockOnLoad = false;
			bShouldChangeVisibility = true;
			break;
		case SVB_VisibilityBlockingOnLoad:
			bShouldBeVisible = true;
			bShouldBlockOnLoad = true;
			bShouldChangeVisibility = true;
			break;
		case SVB_BlockingOnLoad:
			bShouldBeVisible = false;
			bShouldBlockOnLoad = true;
			bShouldChangeVisibility = false;
			break;
		default:
			UE_LOG(LogLevel, Fatal, TEXT("Unsupported usage %i"), (int32)Usage);
		}
	}

	FVisibleLevelStreamingSettings& operator|=(const FVisibleLevelStreamingSettings& B)
	{
		bShouldBeVisible |= B.bShouldBeVisible;
		bShouldBlockOnLoad |= B.bShouldBlockOnLoad;
		bShouldChangeVisibility |= B.bShouldChangeVisibility;
		return *this;
	}

	bool AllSettingsEnabled() const
	{
		return bShouldBeVisible && bShouldBlockOnLoad;
	}

	bool ShouldBeVisible(bool bCurrentShouldBeVisible) const
	{
		if (bShouldChangeVisibility)
		{
			return bShouldBeVisible;
		}
		else
		{
			return bCurrentShouldBeVisible;
		}
	}

	bool ShouldBlockOnLoad() const
	{
		return bShouldBlockOnLoad;
	}

private:
	/** Whether level should be visible.						*/
	bool bShouldBeVisible;
	/** Whether level should block on load.						*/
	bool bShouldBlockOnLoad;
	/** Whether existing visibility settings should be changed. */
	bool bShouldChangeVisibility;
};


static bool IsInStreamingLevelVolume(APlayerController* PC, ULevelStreaming* LevelStreamingObject)
{
	UWorld* World = PC->GetWorld();

	FVector ViewLocation(0, 0, 0);
	FRotator ViewRotation(0, 0, 0);
	PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

	TMap<AVolume*, bool> VolumeMap;

	// For each streaming volume associated with this level . . .
	for (int32 i = 0; i < LevelStreamingObject->EditorStreamingVolumes.Num(); ++i)
	{
		ALevelStreamingVolume* StreamingVolume = LevelStreamingObject->EditorStreamingVolumes[i];
		if (StreamingVolume && !StreamingVolume->bEditorPreVisOnly && !StreamingVolume->bDisabled)
		{
			bool bViewpointInVolume;
			bool* bResult = VolumeMap.Find(StreamingVolume);
			if (bResult)
			{
				// This volume has already been considered for another level.
				bViewpointInVolume = *bResult;
			}
			else
			{
				// Compute whether the viewpoint is inside the volume and cache the result.
				bViewpointInVolume = StreamingVolume->EncompassesPoint(ViewLocation);

				VolumeMap.Add(StreamingVolume, bViewpointInVolume);
			}

			if (bViewpointInVolume)
			{
				return true;
			}
		}
	}

	return false;
}

void AVCPlayerController::ClientUpdateLevelStreamingStatus_Implementation(FName PackageName, bool bNewShouldBeLoaded,
	bool bNewShouldBeVisible, bool bNewShouldBlockOnLoad, int32 LODIndex, FNetLevelVisibilityTransactionId TransactionId, bool bNewShouldBlockOnUnload)
{
	UE_LOG(LogTemp, Warning, TEXT("Level: %s, bNewShouldBeLoaded: %s, bNewShouldBeVisible: %s"), *PackageName.ToString(), A(bNewShouldBeLoaded), A(bNewShouldBeVisible));

	PackageName = NetworkRemapPath(PackageName, true);

	UWorld* World = GetWorld();

	// Distance dependent streaming levels should be controlled by client only
	if (World && World->WorldComposition)
	{
		if (World->WorldComposition->IsDistanceDependentLevel(PackageName))
		{
			return;
		}
	}

	// Search for the streaming level object by name
	ULevelStreaming* LevelStreamingObject = FLevelUtils::FindStreamingLevel(World, PackageName);

	// Skip if streaming level object doesn't allow replicating the status
	if (LevelStreamingObject && !LevelStreamingObject->CanReplicateStreamingStatus())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant replicate streaming status?"));
		return;
	}

	// if we're about to commit a map change, we assume that the streaming update is based on the to be loaded map and so defer it until that is complete
	if (GEngine->ShouldCommitPendingMapChange(World))
	{
		GEngine->AddNewPendingStreamingLevel(World, PackageName, bNewShouldBeLoaded, bNewShouldBeVisible, LODIndex);
	}
	else if (LevelStreamingObject)
	{
		const bool bShouldBeLoaded = IsInStreamingLevelVolume(this, LevelStreamingObject) || VisibleLevels.Contains(PackageName);

		if (GetPlayerState<APlayerState>())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s: bShouldBeLoaded: %s"), *GetPlayerState<APlayerState>()->GetPlayerName(), bShouldBeLoaded ? TEXT("true") : TEXT("false"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("bShouldBeLoaded: %s"), bShouldBeLoaded ? TEXT("true") : TEXT("false"));
		}

		// If we're unloading any levels, we need to request a one frame delay of garbage collection to make sure it happens after the level is actually unloaded
		if (LevelStreamingObject->ShouldBeLoaded() && !bNewShouldBeLoaded)
		{
			GEngine->DelayGarbageCollection();
		}

		LevelStreamingObject->SetShouldBeLoaded((!InvisibleLevels.Contains(PackageName) && bNewShouldBeLoaded) && bShouldBeLoaded);
		LevelStreamingObject->SetShouldBeVisible((!InvisibleLevels.Contains(PackageName) && bNewShouldBeVisible) && bShouldBeLoaded);
		LevelStreamingObject->bShouldBlockOnLoad = bNewShouldBlockOnLoad;
		LevelStreamingObject->bShouldBlockOnUnload = bNewShouldBlockOnUnload;
		LevelStreamingObject->SetLevelLODIndex(LODIndex);
		LevelStreamingObject->UpdateNetVisibilityTransactionState((!InvisibleLevels.Contains(PackageName) && bNewShouldBeVisible) && bShouldBeLoaded, TransactionId);
	}
	else
	{
		UE_LOG(LogStreaming, Log, TEXT("Unable to find streaming object %s"), *PackageName.ToString());
	}
}

void AVCPlayerController::AddVisibleLevel(const TSoftObjectPtr<UWorld> Level)
{
	const FName LevelPathString = Level.ToSoftObjectPath().GetAssetPath().GetPackageName(); // FPaths::GetPath(Level.ToSoftObjectPath().GetAssetPath().ToString()) / Level.GetAssetName();
	VisibleLevels.AddUnique(LevelPathString);
	InvisibleLevels.Remove(LevelPathString);
}

void AVCPlayerController::AddInvisibleLevel(const TSoftObjectPtr<UWorld> Level)
{
	const FName LevelPathString = Level.ToSoftObjectPath().GetAssetPath().GetPackageName();
	InvisibleLevels.AddUnique(LevelPathString);
	VisibleLevels.Remove(LevelPathString);
}

void AVCPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVCPlayerController, VisibleLevels);
	DOREPLIFETIME(AVCPlayerController, InvisibleLevels);
}