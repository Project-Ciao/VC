// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VCFunctionLibrary.generated.h"

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
};
