// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "VCCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class VC_API UVCCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UVCCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void InitializeComponent() override;

	UFUNCTION()
	virtual void OnGameplayTagCountChange(const FGameplayTag Tag, int32 Amount);

	UPROPERTY(EditAnywhere, Category = "VC Character Movement Component")
	FGameplayTagQuery MovementBlockedQuery;
};
