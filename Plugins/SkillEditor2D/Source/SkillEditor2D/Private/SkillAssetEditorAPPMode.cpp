// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditorAPPMode.h"

#include "SkillAssetBPGraphTabSummoner.h"
#include "SkillAssetEditorSequenceTabSummoner.h"
#include "SkillAssetPropertyTabSummoner.h"
#include "SkillEditorPreviewSummoner.h"

const FName SkillAssetEditorAPPMode::SKAModeID(TEXT("SKAMode"));

SkillAssetEditorAPPMode::SkillAssetEditorAPPMode
(TSharedPtr<class FSkillAssetEditor> InEditor, FName InModeName):
FApplicationMode(InModeName
)
{
	AssetEditor=InEditor;
	SkillAssetTabFactories.RegisterFactory
	(MakeShareable(new SkillAssetPropertyTabSummoner(InEditor)));
	SkillAssetTabFactories.RegisterFactory
	(MakeShareable(new SkillEditorPreviewSummoner(InEditor)));
	// SkillAssetTabFactories.RegisterFactory
	// (MakeShareable(new SkillAssetBPGraphTabSummoner(InEditor)));
	SkillAssetTabFactories.RegisterFactory
	(MakeShareable(new SkillAssetEditorSequenceTabSummoner(InEditor)));
	
	check(AssetEditor.IsValid());
	TabLayout=FTabManager::NewLayout("SkillAssetEditor_Layout_v1")
	->AddArea
	(
		// Create a vertical area and spawn the toolbar
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			
			FTabManager::NewStack()
			->SetSizeCoefficient(0.1f)
			->SetHideTabWell(true)
			->AddTab(AssetEditor.Pin()->GetToolbarTabId(), ETabState::OpenedTab)
		)
		->Split
		(
			// Split the tab and pass the tab id to the tab spawner
			FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
			->Split
			(
			FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
			->Split
			(
			FTabManager::NewStack()
			 ->AddTab(FSkillAssetEditor::PreviewTabId, ETabState::OpenedTab)
			)
			->Split
			(
		   FTabManager::NewStack()
			->AddTab(FSkillAssetEditor::PropertiesPanelTabID, ETabState::OpenedTab)
			)
			
			
			)
			->Split
			(
			FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
			->Split
			(
			FTabManager::NewStack()
			 ->AddTab(FSkillAssetEditor::GraphCanvasId, ETabState::OpenedTab)
			 
			)
			->Split
			(
		   FTabManager::NewStack()
			->AddTab(FSkillAssetEditor::SequencerAreaTabID, ETabState::OpenedTab)
			)
			)
		)
	);
 

	
}

void SkillAssetEditorAPPMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FSkillAssetEditor> Editor = AssetEditor.Pin();

	// Tool bar tab
	
	Editor->RegisterToolbarTab(InTabManager.ToSharedRef());

	//Register other factories
	Editor->PushTabFactories(SkillAssetTabFactories);

	// Graph tab
	Editor->DocumentManager->RegisterDocumentFactory(MakeShareable(new SkillAssetBPGraphTabSummoner(Editor)));

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void SkillAssetEditorAPPMode::AddTabFactory(FCreateWorkflowTabFactory FactoryCreator)
{
	if(FactoryCreator.IsBound())
	{
		SkillAssetTabFactories.RegisterFactory(FactoryCreator.Execute(AssetEditor.Pin()));
	}
	
}

void SkillAssetEditorAPPMode::RemoveTabFactory(FName TabFactoryID)
{
	SkillAssetTabFactories.UnregisterFactory(TabFactoryID);
}

void SkillAssetEditorAPPMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void SkillAssetEditorAPPMode::PostActivateMode()
{
	check(AssetEditor.IsValid())
	AssetEditor.Pin()->InvokeSkillAssetEventBPGraphTab();
}

SkillAssetEditorAPPMode::~SkillAssetEditorAPPMode()
{
}
