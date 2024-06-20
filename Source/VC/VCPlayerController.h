// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VC_API AVCPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void ClientUpdateLevelStreamingStatus_Implementation(FName PackageName, bool bNewShouldBeLoaded, 
		bool bNewShouldBeVisible, bool bNewShouldBlockOnLoad, int32 LODIndex, FNetLevelVisibilityTransactionId TransactionId, bool bNewShouldBlockOnUnload) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VC|Player Controller")
	TArray<FName> VisibleLevels;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "VC|Player Controller")
	TArray<FName> InvisibleLevels;

	UFUNCTION(BlueprintCallable, Category = "VC|Player Controller")
	void AddVisibleLevel(const TSoftObjectPtr<UWorld> Level);

	UFUNCTION(BlueprintCallable, Category = "VC|Player Controller")
	void AddInvisibleLevel(const TSoftObjectPtr<UWorld> Level);
};
