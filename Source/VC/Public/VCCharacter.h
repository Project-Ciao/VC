// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "VCCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

UCLASS()
class VC_API AVCCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AVCCharacter(const FObjectInitializer& ObjectInitializer);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; };

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
		UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GAS, meta = (AllowPrivateAccess = "true"))
		UAttributeSet* AttributeSet;

	virtual void PossessedBy(AController* NewController) override;
	virtual void NotifyControllerChanged() override;
};
