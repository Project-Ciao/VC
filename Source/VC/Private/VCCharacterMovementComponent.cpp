// Fill out your copyright notice in the Description page of Project Settings.


#include "VCCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UVCCharacterMovementComponent::UVCCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UVCCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		Interface->GetAbilitySystemComponent()->RegisterGenericGameplayTagEvent().AddUObject(this, &ThisClass::OnGameplayTagCountChange);
	}
}

void UVCCharacterMovementComponent::OnGameplayTagCountChange(const FGameplayTag Tag, int32 Amount)
{
	IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(GetOwner());
	if (Interface == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Interface->GetAbilitySystemComponent();
	if (ASC == nullptr)
	{
		return;
	}

	FGameplayTagContainer OwnerTags;
	ASC->GetOwnedGameplayTags(OwnerTags);

	if (OwnerTags.MatchesQuery(MovementBlockedQuery))
	{
		if (MovementMode != EMovementMode::MOVE_None)
		{
			SetMovementMode(EMovementMode::MOVE_None);
		}
	}
	else
	{
		if (MovementMode == EMovementMode::MOVE_None)
		{
			SetMovementMode(EMovementMode::MOVE_Falling);
		}
	}
}
