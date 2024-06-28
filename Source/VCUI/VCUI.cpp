// Fill out your copyright notice in the Description page of Project Settings.

#include "VCUI.h"
#include "VCLogging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FVCUI"

void FVCUI::StartupModule()
{
}

void FVCUI::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVCUI, VCUI);