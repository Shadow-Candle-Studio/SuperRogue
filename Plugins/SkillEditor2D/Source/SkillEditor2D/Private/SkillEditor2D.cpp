// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillEditor2D.h"

#define LOCTEXT_NAMESPACE "FSkillEditor2DModule"

void FSkillEditor2DModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSkillEditor2DModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSkillEditor2DModule, SkillEditor2D)