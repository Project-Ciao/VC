// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractableComponent.h"

#include "Interact/InteractComponent.h"
#include "Interact/InteractSettings.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	bIsUseable = true;
	SetIsReplicatedByDefault(true);
}

bool UInteractableComponent::IsUseable_Implementation(UInteractComponent* InteractComponent)
{
	if (IsUseableDelegate.IsBound())
	{
		bool bUseableDelegate = IsUseableDelegate.Execute(this, InteractComponent);
		return bIsUseable && bUseableDelegate;
	}

	return bIsUseable;
}

FText UInteractableComponent::GetUseName_Implementation(UInteractComponent* InteractComponent)
{
	if (GetUseNameDelegate.IsBound())
	{
		return GetUseNameDelegate.Execute(this, InteractComponent);
	}

	return UseName;
}

void UInteractableComponent::BeginUse_Implementation(UInteractComponent* InteractComponent)
{
	OnBeginUse.Broadcast(InteractComponent);
}

void UInteractableComponent::EndUse_Implementation(UInteractComponent* InteractComponent)
{
	OnEndUse.Broadcast(InteractComponent);
}

void UInteractableComponent::BeginHighlight_Implementation(UInteractComponent* InteractComponent)
{
	UInteractSettings::GetMutable()->BeginHighlight(InteractComponent, this);

	OnBeginHighlight.Broadcast(InteractComponent);
}

void UInteractableComponent::EndHighlight_Implementation(UInteractComponent* InteractComponent)
{
	UInteractSettings::GetMutable()->EndHighlight(InteractComponent, this);

	OnEndHighlight.Broadcast(InteractComponent);
}
