// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/VCFunctionLibrary.h"

#include "Inkpot/InkpotStory.h"

UObject* UVCFunctionLibrary::GetCDO(UClass* Class)
{
	if (Class)
	{
		return Class->ClassDefaultObject;
	}

	return nullptr;
}

void UVCFunctionLibrary::BindExternalStoryFunctionByName(UInkpotStory* Story, const FString& InStoryFunctionName, UObject* Object, FName InObjectFunctionName, bool bInLookAheadSafe)
{
	if (Story == nullptr)
	{
		return;
	}

	if (Object == nullptr)
	{
		return;
	}

	UFunction* const Func = Object->FindFunction(InObjectFunctionName);
	if (Func)
	{
		FInkpotExternalFunction Delegate;
		Delegate.BindUFunction(Object, InObjectFunctionName);
		Story->BindExternalFunction(InStoryFunctionName, Delegate, bInLookAheadSafe);
	}
}

UWorld* UVCFunctionLibrary::GetWorld(UObject* WorldContextObject)
{
	return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
}

int32 UVCFunctionLibrary::Wrap(int32 Value, int32 Min, int32 Max)
{
	Value -= Min;
	Max -= Min;
	if (Max == 0)
	{
		Max = 1;
	}
	return ((Value % Max + Max) % Max) + Min;
}

bool UVCFunctionLibrary::BindEnhancedInputAction(AActor* Actor, UInputAction* Action, ETriggerEvent TriggerEvent, FVCInputActionBinding Delegate)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Actor->InputComponent))
	{
		const FName FunctionName = Delegate.GetFunctionName();
		if (FunctionName != NAME_None)
		{
			EnhancedInputComponent->BindAction(Action, TriggerEvent, Delegate.GetUObject(), FunctionName);
			return true;
		}
	}

	return false;
}
