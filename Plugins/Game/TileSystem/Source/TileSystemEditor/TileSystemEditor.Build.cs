// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TileSystemEditor : ModuleRules
{
	public TileSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"TileSystem",
				"UnrealEd",
				"EditorFramework",
				"InputCore",
				"DeveloperSettings",
				"UMG"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
		);
	}
}
