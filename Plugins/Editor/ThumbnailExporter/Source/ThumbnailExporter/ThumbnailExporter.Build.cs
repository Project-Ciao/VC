// Copyright 2023 Big Cat Energising. All Rights Reserved.

using UnrealBuildTool;

public class ThumbnailExporter : ModuleRules
{
	public ThumbnailExporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "DeveloperSettings"
            }
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"RHI",
				"RenderCore"
			}
		);

		//PrivateIncludePaths.AddRange(
		//	new string[] {
		//		System.IO.Path.Combine(GetModuleDirectory("Renderer"), "Private"), //required for PostProcessTonemap.h
		//	}
		//);
	}
}
