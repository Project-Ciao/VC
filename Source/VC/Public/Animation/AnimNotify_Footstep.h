// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AnimNotify_Footstep.generated.h"

struct FMaterialFootstep;

/**
 * 
 */
UCLASS()
class VC_API UAnimNotify_Footstep : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_Footstep(const FObjectInitializer& ObjectInitializer);

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override;

	virtual bool TraceFootstep(FMaterialFootstep& Footstep);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
	FName FootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float TraceLength;

	// The half size of the box used for box tracing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FVector TraceHalfSize;

	// Step, jump, land, etc
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep", meta = (Categories = "VC"))
	FGameplayTag FootstepType;

	// Left, right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep", meta = (Categories = "VC"))
	FGameplayTag Foot;
	
};
