// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SuperRogue : ModuleRules
{
	public SuperRogue(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core","AssetTools" ,"CoreUObject", "Engine", "InputCore", "SpinePlugin", "ProceduralMeshComponent","LevelSequenceEditor" });

		PrivateDependencyModuleNames.AddRange(new string[] { "LevelSequenceEditor" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
