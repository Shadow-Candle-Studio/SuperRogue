﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ApplicationMode.h"
#include "BlueprintEditorModes.h"
#include "SkillAssetEditor.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillAssetEditorAPPMode : public FBlueprintEditorApplicationMode
{
public:
	
	SkillAssetEditorAPPMode(TSharedPtr<class FSkillAssetEditor> InEditor, FName InModeName);


	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void AddTabFactory(FCreateWorkflowTabFactory FactoryCreator) override;
	virtual void RemoveTabFactory(FName TabFactoryID) override;
	
	virtual void PostActivateMode() override;
	
	~SkillAssetEditorAPPMode();
	static const FName SKAModeID;
	static FText GetLocalizedMode(const FName InMode)
	{
		static TMap< FName, FText > LocModes;

		if (LocModes.Num() == 0)
		{
			LocModes.Add(SKAModeID, NSLOCTEXT("SkillAssetBlueprintEditorModes", "SkillAssetBlueprintEditorMode", "SkillAsset Blueprint"));
		}

		check(InMode != NAME_None);
		const FText* OutDesc = LocModes.Find(InMode);
		check(OutDesc);
		return *OutDesc;
	}
protected:
	TWeakPtr<FSkillAssetEditor> AssetEditor;
	FWorkflowAllowedTabSet SkillAssetTabFactories;
};
