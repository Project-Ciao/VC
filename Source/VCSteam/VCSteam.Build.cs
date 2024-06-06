// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class VCSteam : ModuleRules
{
	public VCSteam(ReadOnlyTargetRules Target) : base(Target)
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
				
			});

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
    }
}
