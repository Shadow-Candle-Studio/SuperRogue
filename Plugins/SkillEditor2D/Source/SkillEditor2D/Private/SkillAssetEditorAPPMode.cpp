// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditorAPPMode.h"

const FName SkillAssetEditorAPPMode::SKAModeID(TEXT("SKAModeName"));

SkillAssetEditorAPPMode::SkillAssetEditorAPPMode(TSharedPtr<class FSkillAssetEditor> InEditor, FName InModeName): FApplicationMode(InModeName
)
{
	
}

void SkillAssetEditorAPPMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void SkillAssetEditorAPPMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void SkillAssetEditorAPPMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}

SkillAssetEditorAPPMode::~SkillAssetEditorAPPMode()
{
}
