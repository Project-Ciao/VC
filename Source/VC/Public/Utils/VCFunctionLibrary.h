// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EnhancedInputComponent.h"
#include "VCFunctionLibrary.generated.h"

class UInkpotStory;
class UInputAction;

DECLARE_DYNAMIC_DELEGATE_OneParam(FVCInputActionBinding, const FInputActionValue&, InputActionValue);

/**
 * 
 */
UCLASS()
class VC_API UVCFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "VC|Function Library", meta = (KeyWords = "Class Default Object GetClassDefaultObject", DeterminesOutputType = "Class"))
	static UObject* GetCDO(UClass* Class);

	UFUNCTION(BlueprintCallable, Category = "Inkpot|Story")
	static void BindExternalStoryFunctionByName(UInkpotStory* Story, const FString& InStoryFunctionName, UObject* Object, FName InObjectFunctionName, bool bInLookAheadSafe);

	UFUNCTION(BlueprintPure, Category = "VC|Function Library", meta = (WorldContext = "WorldContextObject"))
	static UWorld* GetWorld(UObject* WorldContextObject);

	// Wraps the value between [Min, Max]
	UFUNCTION(BlueprintPure, Category = "VC|Function Library")
	static int32 Wrap(int32 Value, int32 Min, int32 Max);

	UFUNCTION(BlueprintCallable, Category = "VC|Function Library")
	static bool BindEnhancedInputAction(AActor* Actor, UInputAction* Action, ETriggerEvent TriggerEvent, FVCInputActionBinding Delegate);
};
