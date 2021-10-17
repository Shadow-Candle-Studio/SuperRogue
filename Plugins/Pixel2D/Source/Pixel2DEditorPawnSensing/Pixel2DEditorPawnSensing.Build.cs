// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Pixel2DEditorPawnSensing : ModuleRules
{
	public Pixel2DEditorPawnSensing(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("Pixel2DEditorPawnSensing/Private");
        PrivateIncludePaths.Add("Pixel2D/Classes");

        PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"ApplicationCore",
				"Json",
				"Slate",
				"SlateCore",
				"Engine",
				"InputCore",
				"UnrealEd", // for FAssetEditorManager
				"KismetWidgets",
				"Kismet",  // for FWorkflowCentricApplication
                "KismetCompiler",
				"PropertyEditor",
				"RenderCore",
				"Paper2D",
                "Paper2DEditor",
                "Pixel2DEditor",
				"ContentBrowser",
				"WorkspaceMenuStructure",
				"EditorStyle",
				"MeshPaint",
				"EditorWidgets",
				"Projects",
                "AnimationBlueprintEditor",
                "AssetTools",
                "AnimGraph",
                "BlueprintGraph",
                "GraphEditor",
                "Pixel2D"
			});

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"Settings",
				"IntroTutorials",
				"AssetTools",
				"LevelEditor",
                "AnimationBlueprintEditor",
                "AnimGraph",
                "BlueprintGraph",
                "GraphEditor",
                "KismetCompiler",
            });

	}
}
