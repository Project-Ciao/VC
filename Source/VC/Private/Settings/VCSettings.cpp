// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/VCSettings.h"

#include "InteractiveFoliage/InteractiveFoliageManager.h"

UVCSettings::UVCSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InteractiveFoliageManagerClass = AInteractiveFoliageManager::StaticClass();
}
