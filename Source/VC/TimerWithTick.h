// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "TimerWithTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnTimerTickOutputPin, float, TimerLength, float, TimeLeft, float, TimeElapsed, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerFinishOutputPin);

/**
 * 
 */
UCLASS()
class VC_API UTimerWithTick : public UBlueprintAsyncActionBase, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnTimerTickOutputPin OnTick;

	UPROPERTY(BlueprintAssignable)
	FOnTimerFinishOutputPin OnFinish;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Flow Control")
	static UTimerWithTick* SetTimerWithTick(const UObject* WorldContextObject, float TimerLength);

	// UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	//~UBlueprintAsyncActionBase interface

	UPROPERTY()
	float TimerLength;

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	const UObject* WorldContextObject;

	UFUNCTION()
	void OnTimerEnd();

	// FTickableGameObject begin
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	// FTickableGameObject end
};
