// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkinnedFoliageEditor : ModuleRules
{
	public SkinnedFoliageEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "InterchangeCore",
                "InterchangeImport",
                "InterchangeEngine",
                "InterchangeNodes",
                "InterchangeCommonParser",
                "InterchangeFbxParser",
				"InterchangePipelines",
                "InterchangeFactoryNodes",
				"MeshDescription",
				"StaticMeshDescription"
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
