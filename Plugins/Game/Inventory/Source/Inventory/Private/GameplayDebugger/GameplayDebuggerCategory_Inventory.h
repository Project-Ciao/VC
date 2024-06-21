// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#if WITH_GAMEPLAY_DEBUGGER

#include "CoreMinimal.h"
#include "GameplayDebuggerCategory.h"

class APlayerController;
class AActor;

class FGameplayDebuggerCategory_Inventory : public FGameplayDebuggerCategory
{
public:
	FGameplayDebuggerCategory_Inventory();
	void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	
	static TSharedRef<FGameplayDebuggerCategory> MakeInstance();
};

#endif // WITH_GAMEPLAY_DEBUGGER
