// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ApplicationMode.h"
#include "SkillAssetEditor.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillAssetEditorAPPMode : public FApplicationMode
{
public:
	
	SkillAssetEditorAPPMode(TSharedPtr<class FSkillAssetEditor> InEditor, FName InModeName);
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	
	~SkillAssetEditorAPPMode();
	static const FName SKAModeID;
};
