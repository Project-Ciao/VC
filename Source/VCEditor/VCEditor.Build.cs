// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class VCEditor : ModuleRules
{
	public VCEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine"
			});
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"ToolMenus",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"InputCore",
				"VC"
			});
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				
			});
	}
}
