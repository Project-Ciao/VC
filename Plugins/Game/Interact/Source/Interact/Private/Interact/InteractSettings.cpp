// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact/InteractSettings.h"

void UInteractSettings::BeginHighlight(UInteractComponent* InteractComponent, UInteractableComponent* InteractableComponent)
{
	if (OnBeginHighlightOverride.IsBound())
	{
		OnBeginHighlightOverride.Execute(InteractComponent, InteractableComponent);
	}
	else
	{
		StartCustomDepthHighlight(InteractableComponent, 1);
	}
}

void UInteractSettings::EndHighlight(UInteractComponent* InteractComponent, UInteractableComponent* InteractableComponent)
{
	if (OnEndHighlightOverride.IsBound())
	{
		OnEndHighlightOverride.Execute(InteractComponent, InteractableComponent);
	}
	else
	{
		EndCustomDepthHighlight(InteractableComponent, 1);
	}
}

static TArray<UPrimitiveComponent*> GetComponentsToHighlight(const UInteractableComponent* InteractableComponent)
{
	TArray<UPrimitiveComponent*> OwnerComponents;

	TArray<UPrimitiveComponent*> HighlightComponents = InteractableComponent->GetHighlightComponents();
	if (HighlightComponents.Num() > 0)
	{
		OwnerComponents = HighlightComponents;
	}
	else
	{
		InteractableComponent->GetOwner()->GetComponents<UPrimitiveComponent>(OwnerComponents);
	}

	return OwnerComponents;
}

void UInteractSettings::StartCustomDepthHighlight(UInteractableComponent* InteractableComponent, int32 BitMask) const
{
	TArray<UPrimitiveComponent*> OwnerComponents = GetComponentsToHighlight(InteractableComponent);

	for (UPrimitiveComponent* OwnerComponent : OwnerComponents)
	{
		if (OwnerComponent == nullptr)
		{
			continue;
		}
		OwnerComponent->SetRenderCustomDepth(true);
		OwnerComponent->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_255);
		OwnerComponent->SetCustomDepthStencilValue(BitMask | OwnerComponent->CustomDepthStencilValue);
	}
}

void UInteractSettings::EndCustomDepthHighlight(UInteractableComponent* InteractableComponent, int32 BitMask) const
{
	TArray<UPrimitiveComponent*> OwnerComponents = GetComponentsToHighlight(InteractableComponent);

	for (UPrimitiveComponent* OwnerComponent : OwnerComponents)
	{
		if (OwnerComponent == nullptr)
		{
			continue;
		}

		OwnerComponent->SetCustomDepthStencilValue(~(BitMask) &OwnerComponent->CustomDepthStencilValue);
		if (OwnerComponent->CustomDepthStencilValue == 0)
		{
			OwnerComponent->SetRenderCustomDepth(false);
		}
	}
}

const UInteractSettings* UInteractSettings::Get()
{
    return GetDefault<UInteractSettings>();
}

UInteractSettings* UInteractSettings::GetMutable()
{
	return GetMutableDefault<UInteractSettings>();
}
