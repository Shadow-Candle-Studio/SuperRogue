// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ApplicationMode.h"
#include "BlueprintEditorModes.h"
#include "SkillAssetEditor.h"
#include "SkillEditorPreviewSummoner.h"

class SkillEditorPreviewSummoner;
/**
 * 
 */
class SKILLEDITOR2D_API SkillAssetEditorAPPMode : public FBlueprintEditorApplicationMode
{
public:

	//Initialization process of this SKAmode
	SkillAssetEditorAPPMode(TSharedPtr<class FSkillAssetEditor> InEditor);

	//Register all Tab factories
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	TSharedPtr<class SKillAssetPriveiwScene>& GetThePreviewSceneInsideTheMode();
	//UUID for Mod
	static const FName SKAModeID;
	
	TSharedPtr<class SkillEditorPreviewSummoner> PreviewTabFactory;
	TSharedPtr<class SkillAssetPropertyTabSummoner> PropertyTabFactory;

	TSharedPtr<class SkillAssetEditorSequenceTabSummoner> SequencerTabFactory;
	//Localized mode searching
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

	//Extra tab factories you can extend in this editor
	FWorkflowAllowedTabSet SkillAssetTabFactories;
};
