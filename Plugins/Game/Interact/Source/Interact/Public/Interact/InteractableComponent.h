// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

class UInteractComponent;

UENUM(BlueprintType)
enum class EUseReplicationPolicy : uint8 {
	RunLocally	UMETA(DisplayName = "Run locally (Non replicated)"),
	Server		UMETA(DisplayName = "Run on server (Replicated)"),
	Both		UMETA(DisplayName = "Run on both client and server"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUse, UInteractComponent*, InteractComponent);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FIsInteractableUseable, UInteractableComponent*, InteractableComponent, UInteractComponent*, InteractComponent);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FText, FGetInteractableUseName, UInteractableComponent*, InteractableComponent, UInteractComponent*, InteractComponent);

/**
* Put this on an actor to make it interactable. It receives interaction calls from the UInteractComponent
*/
UCLASS( ClassGroup=(Interact), meta=(BlueprintSpawnableComponent), Blueprintable )
class INTERACT_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableComponent();

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Interactable Component")
		bool IsUseable(UInteractComponent* InteractComponent);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Interactable Component")
		FText GetUseName(UInteractComponent* InteractComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable Component")
		void BeginUse(UInteractComponent* InteractComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable Component")
		void EndUse(UInteractComponent* InteractComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable Component")
		void BeginHighlight(UInteractComponent* InteractComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable Component")
		void EndHighlight(UInteractComponent* InteractComponent);

	UPROPERTY(BlueprintAssignable, Category = "Interactable Component")
		FOnUse OnBeginUse;
		
	UPROPERTY(BlueprintAssignable, Category = "Interactable Component")
		FOnUse OnEndUse;

	UPROPERTY(BlueprintAssignable, Category = "Interactable Component")
		FOnUse OnBeginHighlight;

	UPROPERTY(BlueprintAssignable, Category = "Interactable Component")
		FOnUse OnEndHighlight;

	EUseReplicationPolicy GetReplicationPolicy() const { return ReplicationPolicy; }

	TArray<UPrimitiveComponent*> GetInteractTriggers() const { return InteractTriggers; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Component")
		FIsInteractableUseable IsUseableDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Component")
		FGetInteractableUseName GetUseNameDelegate;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactable Component")
		FText UseName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Interactable Component")
		EUseReplicationPolicy ReplicationPolicy;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactable Component")
		bool bIsUseable;

	// If empty, then all overlaps on the parent actor can trigger this interactable component
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interactable Component")
		TArray<UPrimitiveComponent*> InteractTriggers;
};
