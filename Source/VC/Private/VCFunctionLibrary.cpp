// Fill out your copyright notice in the Description page of Project Settings.


#include "VCFunctionLibrary.h"

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
