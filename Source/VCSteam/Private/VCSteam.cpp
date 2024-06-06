// Fill out your copyright notice in the Description page of Project Settings.

#include "VCSteam.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FVCSteam"

void FVCSteam::StartupModule()
{
}

void FVCSteam::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVCSteam, VCSteam);