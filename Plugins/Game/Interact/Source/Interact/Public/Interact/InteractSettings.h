// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Interact/InteractableComponent.h"
#include "InteractSettings.generated.h"

class UInteractComponent;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnHighlight, UInteractComponent*, InteractComponent, UInteractableComponent*, InteractableComponent);

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig)
class INTERACT_API UInteractSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Interact Settings")
		void BeginHighlight(UInteractComponent* InteractComponent, UInteractableComponent* InteractableComponent);

	UFUNCTION(BlueprintCallable, Category = "Interact Settings")
		void EndHighlight(UInteractComponent* InteractComponent, UInteractableComponent* InteractableComponent);

	UFUNCTION(BlueprintCallable, Category = "Interact Settings")
		void StartCustomDepthHighlight(UInteractableComponent* InteractableComponent, int32 BitMask) const;

	UFUNCTION(BlueprintCallable, Category = "Interact Settings")
		void EndCustomDepthHighlight(UInteractableComponent* InteractableComponent, int32 BitMask) const;

	// Override delegate for the default implementation of OnBeginHighlight for all UInteractableComponents
	UPROPERTY()
		FOnHighlight OnBeginHighlightOverride;

	// Override delegate for the default implementation of OnEndHighlight for all UInteractableComponents
	UPROPERTY()
		FOnHighlight OnEndHighlightOverride;

	static const UInteractSettings* Get();
	static UInteractSettings* GetMutable();
};
