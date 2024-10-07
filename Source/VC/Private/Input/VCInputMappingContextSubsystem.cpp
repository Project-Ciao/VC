// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/VCInputMappingContextSubsystem.h"

#include "Input/VCInputMappingContextComponent.h"
#include "EnhancedInputSubsystems.h"

void UVCInputMappingContextSubsystem::PushInputContextMappingComponent(UVCInputMappingContextComponent* Component)
{
	check(Component != nullptr);
	if (InputMappingStack.Contains(Component))
	{
		return;
	}

	InputMappingStack.AddUnique(Component);

	const TArray<FVCInputMap> NewInputMaps = UVCInputMappingContextComponent::GetInputMapsFromComponents(InputMappingStack);
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(InputSubsystem);

	for (const FVCInputMap& InputMap : NewInputMaps)
	{
		if (!InputMaps.Contains(InputMap))
		{
			InputSubsystem->AddMappingContext(InputMap.MappingContext, InputMap.Priority, InputMap.Options);
		}
	}

	InputMaps = NewInputMaps;
}

void UVCInputMappingContextSubsystem::PopInputContextMappingComponent(UVCInputMappingContextComponent* Component)
{
	check(Component != nullptr);
	if (!InputMappingStack.Contains(Component))
	{
		return;
	}

	InputMappingStack.Remove(Component);

	const TArray<FVCInputMap> NewInputMaps = UVCInputMappingContextComponent::GetInputMapsFromComponents(InputMappingStack);
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(InputSubsystem);

	// Iterate over the old input maps, remove newly removed input maps
	for (const FVCInputMap& OldInputMap : InputMaps)
	{
		if (!NewInputMaps.Contains(OldInputMap))
		{
			InputSubsystem->RemoveMappingContext(OldInputMap.MappingContext);
		}
	}

	InputMaps = NewInputMaps;
}
