// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Pixel2D : ModuleRules
{
	public Pixel2D(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("Pixel2D/Private");
        PrivateIncludePaths.Add("Pixel2D/Classes");

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "RenderCore",
                "RHI",
                "SlateCore",
                "Slate",
                "Paper2D"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Renderer",
            }
        );
		
		PrecompileForTargets = PrecompileTargetsType.Any;

    }
}
