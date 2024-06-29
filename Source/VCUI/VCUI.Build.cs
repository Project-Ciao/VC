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
				"VC",
				"FieldNotification",
				"Slate",
				"SlateCore",
				"UMG",
				"CommonUI"
			});
		
		PrivateDependencyModuleNames.AddRange(new string[]
			{
				
			}
		);
	}
}
