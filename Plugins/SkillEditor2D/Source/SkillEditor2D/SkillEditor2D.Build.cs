// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using UnrealBuildTool;

public class SkillEditor2D : ModuleRules
{
	public SkillEditor2D(ReadOnlyTargetRules Target) : base(Target)
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
				"AssetTools",
				"LevelEditor",
				"SkillEditorRuntime",
				//"SkillEditorRenderer"
			}
			
			);	
		PublicIncludePathModuleNames.AddRange(
			new string[]
			{
				"SkillEditorRuntime",
				//"SkillEditorRenderer"
			}


		);
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				//"SkillEditorRenderer"
				//"AssetTools"
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
				"RenderCore",
				"Projects",
				"RHI",
				//"SkillEditorRenderer"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				"AssetTools"
			}
			);
	}
}
