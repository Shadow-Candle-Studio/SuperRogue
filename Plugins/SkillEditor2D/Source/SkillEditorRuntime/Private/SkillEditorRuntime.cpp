// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillEditorRuntime.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "IPluginManager.h"

#include "SkillAssetFactory.h"
#include "SlateStyleRegistry.h"


#define LOCTEXT_NAMESPACE "FSkillEditorRuntimeModule"

void FSkillEditorRuntimeModule::StartupModule()
{
	

	

	
}

void FSkillEditorRuntimeModule::ShutdownModule()
{


	
	
	
}

bool FSkillEditorRuntimeModule::IsGameModule() const
{
	return false;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSkillEditorRuntimeModule, SkillEditorRuntime)