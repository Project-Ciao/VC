// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/VCLevelFunctionLibrary.h"
#include "Level/LevelTeleportExitInterface.h"

FTeleportLocation UVCLevelFunctionLibrary::MakeTeleportLocation(TSoftObjectPtr<UWorld> Level, AActor* Location)
{
	FTeleportLocation Out;
	Out.Level = Level;
	if (Location != nullptr)
	{
		check(Location->GetClass()->ImplementsInterface(ULevelTeleportExitInterface::StaticClass()));
	}
	Out.Location = Location;
	return Out;
}

bool UVCLevelFunctionLibrary::IsTeleportLocationValid(const FTeleportLocation& Location)
{
	return !Location.Level.IsNull() && !Location.Location.IsNull();
}

TSoftObjectPtr<UWorld> UVCLevelFunctionLibrary::GetRemappedTeleportWorld(APlayerController* PC, const FTeleportLocation& Location)
{
	if (PC && !Location.Level.IsNull())
	{
		const FString LocationPackage = Location.Level.GetLongPackageName();
		const FName RemappedLocation = PC->NetworkRemapPath(FName(LocationPackage), true);
		return TSoftObjectPtr<UWorld>(FSoftObjectPath(RemappedLocation));
	}
	return TSoftObjectPtr<UWorld>();
}
