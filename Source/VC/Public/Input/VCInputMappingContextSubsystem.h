// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Input/VCInputMappingContextComponent.h"
#include "VCInputMappingContextSubsystem.generated.h"

class UVCInputMappingContextComponent;

/**
 * 
 */
UCLASS()
class VC_API UVCInputMappingContextSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	// Returns false if the component was already in the InputMappingStacks
	virtual bool PushInputContextMappingComponent(UVCInputMappingContextComponent* Component);

	// Returns false if the component wasn't in the InputMappingStacks
	virtual bool PopInputContextMappingComponent(UVCInputMappingContextComponent* Component);
	
	UPROPERTY(BlueprintReadOnly, Category = "VC|Input Mapping Context System")
	TArray<UVCInputMappingContextComponent*> InputMappingStack;

	UPROPERTY(BlueprintReadOnly, Category = "VC|Input Mapping Context System")
	TArray<FVCInputMap> InputMaps;

};
