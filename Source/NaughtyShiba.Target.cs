// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class NaughtyShibaTarget : TargetRules
{
	public NaughtyShibaTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("NaughtyShiba");
		
		// Compilation optimizations for solo development
		bUseUnityBuild = true;
		bUsePCHFiles = true;
		bForceEnableExceptions = false;
	}
	
}
