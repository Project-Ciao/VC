// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class VCEditorTarget : TargetRules
{
	public VCEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        bUsesSteam = true;

        ExtraModuleNames.AddRange(new string[] { "VC", "VCUI", "VCEditor" });
    }
}
