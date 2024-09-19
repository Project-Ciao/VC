// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"

#include "FootstepMaterialInterface.generated.h"

class USkeletalMeshComponent;
class UAnimSequenceBase;

USTRUCT(BlueprintType)
struct FMaterialFootstep
{
	GENERATED_BODY()

	// Step, jump, land, etc
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep", meta = (Categories = "VC"))
	FGameplayTag FootstepType;

	// Left, right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep", meta = (Categories = "VC"))
	FGameplayTag Foot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	FHitResult Hit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	UAnimSequenceBase* Animation;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFootstepMaterialInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VC_API IFootstepMaterialInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Footstep Material Interface")
	void OnFootstep(const FMaterialFootstep& Footstep);
	
};
