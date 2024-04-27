// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class LiveLinkPluginsEditorTarget : TargetRules
{
	public LiveLinkPluginsEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "LiveLinkPlugins" } );
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
    }
}
