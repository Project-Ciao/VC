// Fill out your copyright notice in the Description page of Project Settings.


#include "VCFunctionLibrary.h"

UObject* UVCFunctionLibrary::GetCDO(UClass* Class)
{
	if (Class)
	{
		return Class->ClassDefaultObject;
	}

	return nullptr;
}