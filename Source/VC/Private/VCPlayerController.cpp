// Fill out your copyright notice in the Description page of Project Settings.


#include "VCPlayerController.h"

#include "GameFramework/PlayerState.h"
#include "Engine/WorldComposition.h"
#include "LevelUtils.h"
#include "Net/UnrealNetwork.h"
#include "Engine/LevelStreamingVolume.h"

UE_DISABLE_OPTIMIZATION
static bool IsInStreamingLevelVolume(const APlayerController* PC, ULevelStreaming* LevelStreamingObject)
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
	//if (GetNetMode() == ENetMode::NM_DedicatedServer || GetNetMode() == ENetMode::NM_Standalone)
	//{
	//	Super::ClientUpdateLevelStreamingStatus_Implementation(PackageName, bNewShouldBeLoaded, bNewShouldBeVisible, bNewShouldBlockOnLoad, LODIndex, TransactionId, bNewShouldBlockOnUnload);
	//	return;
	//}
	//UE_LOG(LogTemp, Warning, TEXT("Level: %s, bNewShouldBeLoaded: %s, bNewShouldBeVisible: %s"), *PackageName.ToString(), A(bNewShouldBeLoaded), A(bNewShouldBeVisible));

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

		// If we're unloading any levels, we need to request a one frame delay of garbage collection to make sure it happens after the level is actually unloaded
		if (LevelStreamingObject->ShouldBeLoaded() && !bNewShouldBeLoaded)
		{
			GEngine->DelayGarbageCollection();
		}

		const bool ShouldBeLoaded = (!InvisibleLevels.Contains(PackageName) && bNewShouldBeLoaded) && bShouldBeLoaded;
		LevelStreamingObject->SetShouldBeLoaded(ShouldBeLoaded);
		LevelStreamingObject->SetShouldBeVisible((!InvisibleLevels.Contains(PackageName) && bNewShouldBeVisible) && bShouldBeLoaded);
		LevelStreamingObject->bShouldBlockOnLoad = bNewShouldBlockOnLoad;
		LevelStreamingObject->bShouldBlockOnUnload = bNewShouldBlockOnUnload;
		LevelStreamingObject->SetLevelLODIndex(LODIndex);
		LevelStreamingObject->UpdateNetVisibilityTransactionState((!InvisibleLevels.Contains(PackageName) && bNewShouldBeVisible) && bShouldBeLoaded, TransactionId);

		if (ShouldBeLoaded)
		{
			if (!LevelStreamingObject->IsLevelLoaded())
			{
				LevelsLoading.AddUnique(LevelStreamingObject);
			}
		}
		else
		{
			LevelsLoading.Remove(LevelStreamingObject);
		}
	}
	else
	{
		UE_LOG(LogStreaming, Log, TEXT("Unable to find streaming object %s"), *PackageName.ToString());
	}
}

void AVCPlayerController::AddVisibleLevel(const TSoftObjectPtr<UWorld> Level)
{
	const FName LevelPathString = Level.ToSoftObjectPath().GetAssetPath().GetPackageName(); // FPaths::GetPath(Level.ToSoftObjectPath().GetAssetPath().ToString()) / Level.GetAssetName();
	const FName RemappedLevelPath = NetworkRemapPath(LevelPathString, true);
	VisibleLevels.AddUnique(LevelPathString);
	VisibleLevels.AddUnique(RemappedLevelPath);
	InvisibleLevels.Remove(LevelPathString);
	InvisibleLevels.Remove(RemappedLevelPath);
}

void AVCPlayerController::AddInvisibleLevel(const TSoftObjectPtr<UWorld> Level)
{
	const FName LevelPathString = Level.ToSoftObjectPath().GetAssetPath().GetPackageName();
	const FName RemappedLevelPath = NetworkRemapPath(LevelPathString, true);
	InvisibleLevels.AddUnique(LevelPathString);
	InvisibleLevels.AddUnique(RemappedLevelPath);
	VisibleLevels.Remove(LevelPathString);
	VisibleLevels.Remove(RemappedLevelPath);
}

void AVCPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (int32 i = LevelsLoading.Num() - 1; i >= 0; --i)
	{
		if (LevelsLoading[i].IsValid() == false)
		{
			LevelsLoading.RemoveAt(i);
		}
		else
		{
			if (LevelsLoading[i]->IsLevelLoaded())
			{
				LevelsLoading.RemoveAt(i);
			}
		}
	}
}

bool AVCPlayerController::AreAnyLevelsLoading() const
{
	return LevelsLoading.Num() > 0;
}

//void AVCPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	//DOREPLIFETIME(AVCPlayerController, VisibleLevels);
//	//DOREPLIFETIME(AVCPlayerController, InvisibleLevels);
//}
UE_ENABLE_OPTIMIZATION
