// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class LiveLinkPluginsTarget : TargetRules
{
	public LiveLinkPluginsTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "LiveLinkPlugins" } );
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
    }
}
