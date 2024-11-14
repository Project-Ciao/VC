// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/VCFunctionLibrary.h"

#include "Inkpot/InkpotStory.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"

UE_DISABLE_OPTIMIZATION
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

UWorld* UVCFunctionLibrary::K2_GetWorld(UObject* WorldContextObject)
{
	return GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
}

UWorld* UVCFunctionLibrary::K2_GetWorldFromObject(UObject* WorldContextObject)
{
	return K2_GetWorld(WorldContextObject);
}

FName UVCFunctionLibrary::GetLevelNameFromObject(const UObject* Object)
{
	if (Object)
	{
		UWorld* World = Object->GetTypedOuter<UWorld>();
		if (World)
		{
			return World->GetFName();
		}
	}
	return NAME_None;
}

void UVCFunctionLibrary::K2_GetAllActorsOfClassInWorld(UWorld* World, TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors)
{
	UGameplayStatics::GetAllActorsOfClass(World, ActorClass, OutActors);
}

UWorld* UVCFunctionLibrary::GetWorldFromLevelStreamingObject(ULevelStreamingDynamic* LevelStreamingObject)
{
	if (LevelStreamingObject == nullptr)
	{
		return nullptr;
	}
	
	ULevel* Level = LevelStreamingObject->GetLoadedLevel();
	if (Level == nullptr)
	{
		return nullptr;
	}

	return Level->GetTypedOuter<UWorld>();
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

AActor* UVCFunctionLibrary::SpawnActorInLevel(UWorld* Level, TSubclassOf<AActor> Class, const FTransform& Transform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, ESpawnActorScaleMethod TransformScaleMethod, AActor* Owner, APawn* Instigator)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Instigator;
	SpawnParams.SpawnCollisionHandlingOverride = CollisionHandlingOverride;
	SpawnParams.TransformScaleMethod = TransformScaleMethod;
	return Level->SpawnActor<AActor>(Class, Transform);
}
UE_ENABLE_OPTIMIZATION
