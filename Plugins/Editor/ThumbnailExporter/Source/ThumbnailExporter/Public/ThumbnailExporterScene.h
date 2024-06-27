// Copyright 2023 Big Cat Energising. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "ThumbnailHelpers.h"

/**
 * Thumbnail preview scene with support for both blueprints and static meshes
 */
class THUMBNAILEXPORTER_API FThumbnailExporterScene : public FThumbnailPreviewScene
{
public:
	FThumbnailExporterScene(bool bInHideBackgroundMeshes);
	virtual ~FThumbnailExporterScene() = default;

	/** Allocates then adds an FSceneView to the ViewFamily. */
	FSceneView* CreateView(FSceneViewFamily* ViewFamily, int32 X, int32 Y, uint32 SizeX, uint32 SizeY) const;

	/** Returns true if this component can be visualized */
	static bool IsValidComponentForVisualization(UActorComponent* Component);

	/** Sets the static mesh to use in the next CreateView() */
	void SetBlueprint(class UBlueprint* Blueprint);

	/** Refreshes components for the specified blueprint */
	void BlueprintChanged(class UBlueprint* Blueprint);

	/** Sets the static mesh to use in the next CreateView() */
	void SetStaticMesh(class UStaticMesh* StaticMesh);

	/** Sets the skeletal mesh to use in the next CreateView() */
	void SetSkeletalMesh(class USkeletalMesh* InSkeletalMesh);

	/** Sets override materials for the static mesh  */
	void SetOverrideMaterials(const TArray<class UMaterialInterface*>& OverrideMaterials);

	bool GetBackgroundMeshesHidden() const { return bHideBackgroundMeshes; }
	TWeakObjectPtr<class AActor> GetPreviewActor() const { return PreviewActor; }

protected:
	// FThumbnailPreviewScene implementation
	virtual void GetViewMatrixParameters(const float InFOVDegrees, FVector& OutOrigin, float& OutOrbitPitch, float& OutOrbitYaw, float& OutOrbitZoom) const override;

	/** Sets the object (class or blueprint) used in the next CreateView() */
	void SpawnPreviewActor(class UClass* Obj);

	/** Get the scene thumbnail info to use for the object currently being rendered */
	virtual USceneThumbnailInfo* GetSceneThumbnailInfo(const float TargetDistance) const;

	FBoxSphereBounds GetPreviewActorBounds() const;

	/** Clears out any stale actors in this scene if PreviewActor enters a stale state */
	void ClearStaleActors();

	bool bHideBackgroundMeshes;

	int32 NumStartingActors;
	TWeakObjectPtr<class AActor> PreviewActor;

	/** The blueprint that is currently being rendered. NULL when not rendering. */
	TWeakObjectPtr<class UBlueprint> CurrentBlueprint;
};
