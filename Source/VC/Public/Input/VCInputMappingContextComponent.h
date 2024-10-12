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

	FVCInputMap()
		: MappingContext(nullptr), Priority(0), Options()
	{
	}

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

class UVCInputMappingContextComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInputMappingContextPushedPopped, UVCInputMappingContextComponent*, Component, APlayerController*, PC);

UCLASS( Blueprintable, ClassGroup=(Input), meta=(BlueprintSpawnableComponent) )
class VC_API UVCInputMappingContextComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVCInputMappingContextComponent(const FObjectInitializer& ObjectInitializer);

	TArray<FVCInputMap> GetInputMaps() const { return InputMaps; }

	static TArray<FVCInputMap> GetInputMapsFromComponents(const TArray<UVCInputMappingContextComponent*>& Components);

	UFUNCTION(BlueprintCallable, Category = "Input Mapping Context Component")
	bool PushInputMappingContext(APlayerController* Player);

	UFUNCTION(BlueprintCallable, Category = "Input Mapping Context Component")
	bool PopInputMappingContext();

	UFUNCTION(BlueprintCallable, Category = "Input Mapping Context Component")
	void ListenForPawnControllerChange(APawn* Pawn);

	UFUNCTION(BlueprintCallable, Category = "Input Mapping Context Component")
	void UnlistenToPawnControllerChange(APawn* Pawn);

	// Get the pawn to bind input to
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input Mapping Context Component")
	APawn* GetInputPawn();

	UFUNCTION(BlueprintImplementableEvent, Category = "Input Mapping Context Component", meta=(DisplayName="On Input Mapping Context Pushed"))
	void BP_OnInputMappingContextPushed(APlayerController* PC);

	UFUNCTION(BlueprintImplementableEvent, Category = "Input Mapping Context Component", meta = (DisplayName = "On Input Mapping Context Popped"))
	void BP_OnInputMappingContextPopped(APlayerController* PC);

	UPROPERTY(BlueprintAssignable, Category = "Input Mapping Context Component")
	FOnInputMappingContextPushedPopped OnInputMappingContextPushed;

	UPROPERTY(BlueprintAssignable, Category = "Input Mapping Context Component")
	FOnInputMappingContextPushedPopped OnInputMappingContextPopped;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Mapping Context Component")
	TArray<FVCInputMap> InputMaps;

	virtual void BeginPlay() override;
	void OnUnregister() override;

	UFUNCTION()
	void OnReceiveControllerChanged(APawn* Pawn, AController* OldController, AController* NewController);

	// What player the input map is bound to
	UPROPERTY(VisibleInstanceOnly, Category = "Input Mapping Context Component")
	APlayerController* BoundPC;
};
