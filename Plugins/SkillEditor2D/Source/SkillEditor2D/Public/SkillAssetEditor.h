// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowCentricApplication.h"


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
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
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

	virtual USkillAsset* GetSkillAsset() override;
	virtual void SetSkillAsset(USkillAsset* InSkillAsset) override;
	TSharedPtr<class FSceneViewport> Viewport;
	TSharedPtr<class SViewport> ViewportWidget;
	// FSkillEditorViewPortRenderingClient* RenderingClient;
	
private:
	TSharedRef<SDockTab> SpawnPropertiesTab(const FSpawnTabArgs& Args);
	
	TSharedPtr< SDockableTab > PropertiesTab;


	TSharedPtr<class IDetailsView> DetailsView;
	static const FName	ToolkitFName;
	static const FName PropertiesTabId;
	USkillAsset* SkillAsset = nullptr;
	
};