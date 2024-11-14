// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASUtils.h"

#define LOCTEXT_NAMESPACE "FGASUtilsModule"

void FGASUtilsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FGASUtilsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGASUtilsModule, GASUtils)