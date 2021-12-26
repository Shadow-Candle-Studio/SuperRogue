// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditor.h"
#include "SkillEditorPreviewTabBody.h"
#include "USKAInstance.h"

#include "WorkflowTabManager.h"


class USkillAsset;


class SKILLEDITOR2D_API ISkillAssetEditor : public FBlueprintEditor
{
public:
	virtual USkillAsset* GetSkillAsset()=0;
	virtual void SetSkillAsset(USkillAsset* InSkillAsset) =0;
	
};

class SKILLEDITOR2D_API FSkillAssetEditor:
public ISkillAssetEditor
{
public:
	
	
	//Editor initialization
	void InitSkillAssetEditor
	(const EToolkitMode::Type Mode,
	const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	USkillAsset* InSkillAsset);
	//Blueprint interfaces ----begins
	virtual UBlueprint* GetBlueprintObj() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override;
	//Blueprint interfaces ----ends


	//FSkillAssetEditor functions ----begins
	virtual USkillAsset* GetSkillAsset() override;
	virtual void SetSkillAsset(USkillAsset* InSkillAsset) override;
	void TextFuncOncliked();
	void FillsubMenu(FMenuBuilder& Menubuilder);
	void FillToolbar(FToolBarBuilder& ToolBarbuilder);
	void ExtendMenu();
	void ExtendToolBar();
	bool canExecuteBar()const {return true;}
	//FSkillAssetEditor functions ----ends

	//Identifiers for tabs and graphs
	static const FName ToolkitFName;
	static const FName PreviewTabId;
	static const FName GraphCanvasId;
	static const FName SequencerAreaTabID;
	static const FName PropertiesPanelTabID;
	
	TSharedPtr<FUICommandList> SkillAssetExtcommands;
	TSharedPtr<SKillAssetPriveiwScene> GetPreviewSceneDirectly();

private:
	
	

	
	TSharedPtr<class SkillAssetEditorAPPMode> SKAEditorModeInuse;

	USkillAsset* SkillAsset;
protected:
	/** The extender to pass to the level editor to extend it's window menu */
	TSharedPtr<FExtender> MenuExtender;

	/** Toolbar extender */
	TSharedPtr<FExtender> ToolbarExtender;
};


