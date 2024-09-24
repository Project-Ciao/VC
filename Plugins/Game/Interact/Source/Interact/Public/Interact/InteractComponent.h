// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentInteractableComponentsChange, const TArray<UInteractableComponent*>&, InteractableComponents);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentInteractableComponentChange, UInteractableComponent*, OldInteractableComponent, UInteractableComponent*, NewInteractableComponent);

UENUM(BlueprintType)
enum class EInteractComponentTraceMethod : uint8
{
	Overlap UMETA(ToolTip = "Look for actors that overlap the interact component's owner"),
	Trace	UMETA(ToolTip = "Do a line trace for interactable actors"),
	Custom
};

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(EInteractComponentTraceMethod, FGetInteractComponentTraceMethod, UInteractComponent*, InteractComponent);

class UInteractableComponent;

UCLASS( ClassGroup=(Interact), meta=(BlueprintSpawnableComponent), Blueprintable)
class INTERACT_API UInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractComponent();

	// Calls BeginUse on whatever component is currently highlighted. Call this function to interact
	UFUNCTION(BlueprintCallable, Category = "Interact Component")
		void InputBeginUse();

	// Calls EndUse on whatever component is currently highlighted. Call this function to end interaction
	UFUNCTION(BlueprintCallable, Category = "Interact Component")
		void InputEndUse();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Interact Component")
		UPrimitiveComponent* GetOwnerOverlapComponent();

	UPROPERTY(BlueprintAssignable, Category = "Interact Component")
		FOnCurrentInteractableComponentsChange OnCurrentInteractableComponentsChange;

	UPROPERTY(BlueprintAssignable, Category = "Interact Component")
		FOnCurrentInteractableComponentChange OnCurrentInteractableComponentChange;

	// Returns all components that have owners that overlap this owner
	UFUNCTION(BlueprintCallable, Category = "Interact Component")
		TArray<UInteractableComponent*> GetOverlappingInteractableComponents();

	// Traces from the owner's camera (if it can find it) and returns the first component that the trace hits
	UFUNCTION(BlueprintCallable, Category = "Interact Component")
		UInteractableComponent* TraceInteractableComponents();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Interact Component")
		EInteractComponentTraceMethod GetTraceMethod();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Component")
		EInteractComponentTraceMethod TraceMethod;

	UPROPERTY(BlueprintReadWrite, Category = "Interact Component")
		FGetInteractComponentTraceMethod GetInteractComponentTraceMethodDelegate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Component|Trace", meta = (ForceUnits = "cm"))
		float TraceLength;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Component|Trace")
		TEnumAsByte<ECollisionChannel> TraceChannel;

	// Draws the trace if the trace method is Line Trace
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Component|Trace")
		bool bDebugTrace;

	// Resets any currently highlighted actors if the pawn owner is unposessed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Interact Component")
		bool bRemoveHighlightOnPawnOwnerUnposesss;

	// Returns true if this interact component's owner is locally controlled
	UFUNCTION(BlueprintPure, Category = "Interact Component")
		bool IsLocallyControlled() const;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Interact Component")
		TArray<UInteractableComponent*> CurrentInteractableComponents;

	// The component that has had BeginUse called on it
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Interact Component")
		UInteractableComponent* InteractedComponent;

	UFUNCTION()
		void BeginUse(UInteractableComponent* InteractableComponent);

	UFUNCTION()
		void EndUse(UInteractableComponent* InteractableComponent);
		
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_BeginUse(UInteractableComponent* InteractableComponent);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Interact Component")
		void Server_EndUse(UInteractableComponent* InteractableComponent);

	void CurrentInteractableComponentChange(UInteractableComponent* OldInteractableComponent, UInteractableComponent* NewInteractableComponent);

	virtual void InitializeComponent() override;

	UFUNCTION()
	virtual void OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);
};
