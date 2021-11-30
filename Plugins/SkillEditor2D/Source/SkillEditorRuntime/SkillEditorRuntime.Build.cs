// Copyright Epic Games, Inc. All Rights Reserved.
using System;
using UnrealBuildTool;

public class SkillEditorRuntime : ModuleRules
{
	public SkillEditorRuntime(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] 
			{
				
				
			}
			);
		PrivateIncludePathModuleNames.AddRange(
			new string[]
			{
				"Settings",
				"IntroTutorials",
				//"AssetTools",
				"LevelEditor",
				"SkillEditor2D",
				"CurveEditor",
				"SequencerWidgets",
				"Sequencer"
			}
			
		);	
		PublicIncludePathModuleNames.AddRange(
			new string[]
			{
				"SkillEditor2D",
				//"AssetTools",
				"CurveEditor",
				"SequencerWidgets",
				"Sequencer"
			}


		);
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				//"AssetTools",
				"CurveEditor",
				"SequencerWidgets",
				"Sequencer"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Json",
				"Core",
				"CoreUObject",
				"Json",
				"UnrealEd", // for FAssetEditorManager
				"KismetWidgets",
				"Kismet",  // for FWorkflowCentricApplication
				"PropertyEditor",
				"RenderCore",
				"ContentBrowser",
				"WorkspaceMenuStructure",
				"EditorStyle",
				"EditorWidgets",
				"AssetRegistry",
				"AssetTools",
				"CurveEditor",
				"SequencerWidgets",
				"Sequencer"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				//"AssetTools"
			}
			);
		PrivatePCHHeaderFile = "YourModuleNamePrivatePCH.h";
		SharedPCHHeaderFile = "YourModuleNameSharedPCH.h";
	}
}
