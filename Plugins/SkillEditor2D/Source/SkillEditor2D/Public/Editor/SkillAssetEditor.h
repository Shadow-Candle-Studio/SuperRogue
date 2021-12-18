// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SKAUEdGraph.h"
#include "SkillEditorPreviewTabBody.h"
#include "WorkflowCentricApplication.h"
#include "WorkflowTabManager.h"


class USkillAsset;


class SKILLEDITOR2D_API ISkillAssetEditor : public FWorkflowCentricApplication
{
public:
	virtual USkillAsset* GetSkillAsset()=0;
	virtual void SetSkillAsset(USkillAsset* InSkillAsset) =0;
};

class SKILLEDITOR2D_API FSkillAssetEditor:
public ISkillAssetEditor,
public FEditorUndoClient,
public FNotifyHook
{
public:
	//register all tab spawner factory to produce tab when opened
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	//unregister all the tab spawner factory to stop spawning
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	//Editor initialization
	void InitSkillAssetEditor
	(const EToolkitMode::Type Mode,
	const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	USkillAsset* InSkillAsset);
	virtual ~FSkillAssetEditor() override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override;
	
	//register the layout of the editor
    virtual void SetCurrentMode(FName NewMode) override;
	virtual USkillAsset* GetSkillAsset() override;
	virtual void SetSkillAsset(USkillAsset* InSkillAsset) override;
	TSharedPtr<class FSceneViewport> Viewport;
	TSharedPtr<class SViewport> ViewportWidget;
	
	
	
	static const FName ToolkitFName;
	static const FName PreviewTabId;
	static const FName GraphCanvasId;
	static const FName SequencerAreaTabID;
	static const FName PropertiesPanelTabID;
	
	TSharedPtr<FUICommandList> SkillAssetExtcommands;
	void TextFuncOncliked();
	void FillsubMenu(FMenuBuilder& Menubuilder);
	void FillToolbar(FToolBarBuilder& ToolBarbuilder);
	void ExtendMenu();
	 void ExtendToolBar();
	void CreateNewNode();
	bool CanCreateNewNode();
	
	//called mainly by mode instance
	virtual void RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager);
	void InvokeSkillAssetEventBPGraphTab();
	TSharedPtr<FDocumentTracker> DocumentManager;
	
private:
	
	

	
	TSharedPtr<class SkillAssetEditorAPPMode> SKAEditorModeInuse;
	USkillAsset* SkillAsset;

};


