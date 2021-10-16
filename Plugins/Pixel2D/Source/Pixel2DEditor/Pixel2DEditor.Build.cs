// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Pixel2DEditor : ModuleRules
{
	public Pixel2DEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add("Pixel2DEditor/Private");
        PrivateIncludePaths.Add("Pixel2DEditor/Private/Pixel2DAnimEditor");
        PrivateIncludePaths.Add("Pixel2DEditor/Private/Pixel2DAnimGraph");
        PrivateIncludePaths.Add("Pixel2DEditor/Private/Pixel2DAnimGraph/VisualNodes");
        PrivateIncludePaths.Add("Pixel2DEditor/Private/Pixel2DAnimNotify");
        PrivateIncludePaths.Add("Pixel2DEditor/Private/Pixel2DLayers");

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
				"ContentBrowser",
				"WorkspaceMenuStructure",
				"EditorStyle",
				"MeshPaint",
				"EditorWidgets",
				"Projects",
                "AnimationBlueprintEditor",
                "AssetTools",
                "AnimGraph",
				"ToolMenus",
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
				"ToolMenus"
            });

        PrivateIncludePaths.AddRange(
            new string[] {
               // "AnimationBlueprintEditor/Private",
               // "AnimationBlueprintEditor/Private/AnimationStateNodes",
               // "Paper2DEditor/Private",
              //  "AssetTools/Private",
              //  "AnimGraph/Private",
              //  "BlueprintGraph/Private",
             //   "GraphEditor",
             //   "KismetCompiler"
                // ... add other private include paths required here ...
         });

        DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				//"AssetTools"
			});

	}
}
