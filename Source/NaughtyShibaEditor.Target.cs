// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class NaughtyShibaEditorTarget : TargetRules
{
	public NaughtyShibaEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("NaughtyShiba");
		
		bUseUnityBuild = false; // Faster incremental builds in editor
		bUsePCHFiles = true;
		bForceEnableExceptions = false;
	}
}
