// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VCTarget : TargetRules
{
	public VCTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		bUsesSteam = true;

		ExtraModuleNames.AddRange(new string[] { "VC", "VCSteam", "VCUI" });
	}
}
