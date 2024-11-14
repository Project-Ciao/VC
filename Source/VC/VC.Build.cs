// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VC : ModuleRules
{
	public VC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"Inkpot",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"Foliage",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
