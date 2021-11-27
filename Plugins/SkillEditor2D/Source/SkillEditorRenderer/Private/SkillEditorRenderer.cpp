// Copyright Epic Games, Inc. All Rights Reserved.
#include "SkillEditorRenderer.h"
#include "ShaderCore.h"

#include "Interfaces/IPluginManager.h"

void FSkillEditorRendererModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("SkillEditor2D"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/SkillEditor2D"), PluginShaderDir);
}
void FSkillEditorRendererModule::ShutdownModule()
{
	
}





























IMPLEMENT_MODULE(FSkillEditorRendererModule, SkillEditorRenderer)