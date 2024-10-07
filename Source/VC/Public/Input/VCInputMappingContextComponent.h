// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "VCInputMappingContextComponent.generated.h"

class APlayerController;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FVCInputMap
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VC|Input Map")
	UInputMappingContext* MappingContext;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VC|Input Map")
	int32 Priority;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VC|Input Map")
	FModifyContextOptions Options;

	bool operator==(const FVCInputMap& Other) const
	{
		return MappingContext == Other.MappingContext
			&& Priority == Other.Priority
			&& Options.bForceImmediately == Other.Options.bForceImmediately
			&& Options.bIgnoreAllPressedKeysUntilRelease == Other.Options.bIgnoreAllPressedKeysUntilRelease
			&& Options.bNotifyUserSettings == Other.Options.bNotifyUserSettings;
	}

	bool operator!=(const FVCInputMap& Other) const
	{
		return !(*this == Other);
	}
};

UCLASS( ClassGroup=(Input), meta=(BlueprintSpawnableComponent) )
class VC_API UVCInputMappingContextComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVCInputMappingContextComponent(const FObjectInitializer& ObjectInitializer);

	TArray<FVCInputMap> GetInputMaps() const { return InputMaps; }

	static TArray<FVCInputMap> GetInputMapsFromComponents(const TArray<UVCInputMappingContextComponent*>& Components);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping Context Component")
	TArray<FVCInputMap> InputMaps;

	virtual void InitializeComponent() override;
	void OnUnregister() override;

	UFUNCTION()
	void OnReceiveControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	// What player the input map is bound to
	UPROPERTY(VisibleInstanceOnly, Category = "Input Mapping Context Component")
	APlayerController* BoundPC;
};
