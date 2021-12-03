// Copyright Epic Games, Inc. All Rights Reserved.
#include "SkillEditorRenderer.h"
#include "ShaderCore.h"

#include "Interfaces/IPluginManager.h"

void FSkillEditorRenderer::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("SkillEditor2D"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/SkillEditor2D"), PluginShaderDir);

	
}
void FSkillEditorRenderer::ShutdownModule()
{
	
	
}



IMPLEMENT_MODULE(FSkillEditorRenderer, SkillEditorRenderer)
