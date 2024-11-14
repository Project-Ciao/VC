// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VCPlayerController.generated.h"

class ULevelStreaming;
class UPlayerLevelStreamingComponent;

/**
 * 
 */
UCLASS()
class VC_API AVCPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AVCPlayerController(const FObjectInitializer& ObjectInitializer);

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

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "VC|Player Controller")
	virtual bool AreAnyLevelsLoading() const;

protected:
	// A set of levels that have been requested to load, but aren't fully loaded in
	UPROPERTY()
	TArray<TWeakObjectPtr<ULevelStreaming>> LevelsLoading;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VC|Player Controller")
	UPlayerLevelStreamingComponent* LevelStreamingComponent;
};
