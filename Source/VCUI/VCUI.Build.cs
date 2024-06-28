// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class VCUI : ModuleRules
{
	public VCUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core",
				"CoreUObject",
				"Engine",
				"VC"
			});
		
		PrivateDependencyModuleNames.AddRange(new string[]
			{
				
			}
		);
	}
}
