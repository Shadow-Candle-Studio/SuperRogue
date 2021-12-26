// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditorAPPMode.h"
#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "SkillAssetEditorSequenceTabSummoner.h"
#include "SkillAssetPropertyTabSummoner.h"
#include "SkillEditorPreviewSummoner.h"

const FName SkillAssetEditorAPPMode::SKAModeID(TEXT("SKAMode"));

SkillAssetEditorAPPMode::SkillAssetEditorAPPMode
(TSharedPtr<class FSkillAssetEditor> InEditor):
FBlueprintEditorApplicationMode(InEditor, SkillAssetEditorAPPMode::SKAModeID,
	SkillAssetEditorAPPMode::GetLocalizedMode, false, false)
{
	//assign the editor pointer
	MyBlueprintEditor=InEditor;

	//add preview tab factory
	PreviewTabFactory=MakeShareable(new SkillEditorPreviewSummoner(InEditor));
	SkillAssetTabFactories.RegisterFactory
	(PreviewTabFactory);
	//add property showing
	PropertyTabFactory=MakeShareable(new SkillAssetPropertyTabSummoner(InEditor));
	SkillAssetTabFactories.RegisterFactory
	(PropertyTabFactory);
	
	SequencerTabFactory=MakeShareable(new SkillAssetEditorSequenceTabSummoner(InEditor));
	//add sequence tab factory
	SkillAssetTabFactories.RegisterFactory
	(SequencerTabFactory);

	//Add customized layout
	check(MyBlueprintEditor.IsValid());
	TabLayout=FTabManager::NewLayout("SkillAssetEditor_Layout_v1")
			->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				// Top toolbar with compiling button
				FTabManager::NewStack()
				->SetSizeCoefficient(0.186721f)
				->SetHideTabWell(true)
				->AddTab(InEditor->GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				// Main application area
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					// Left side
					FTabManager::NewSplitter()
					->SetSizeCoefficient(0.25f)
					->SetOrientation(Orient_Vertical)
					->Split
					(
						//	Left top - preview tab
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(FSkillAssetEditor::PreviewTabId, ETabState::OpenedTab)
					)
					->Split
					(
					//Left bottom blueprint added variables
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab(FBlueprintEditorTabs::MyBlueprintID, ETabState::OpenedTab)
					->AddTab(FSkillAssetEditor::PropertiesPanelTabID,ETabState::OpenedTab)
					)
				)
				->Split
				(
					// Middle 
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.55f)
					->Split
					(
						// Middle top - document edit area
						FTabManager::NewStack()
						->SetSizeCoefficient(0.8f)
						->AddTab("Document", ETabState::ClosedTab)
					)
					->Split
					(
						// Middle bottom - compiler results & find
						FTabManager::NewStack()
						->SetSizeCoefficient(0.2f)
						->AddTab(FBlueprintEditorTabs::CompilerResultsID, ETabState::ClosedTab)
						->AddTab(FBlueprintEditorTabs::FindResultsID, ETabState::ClosedTab)
					)
				)
				->Split
				(
					// Right side
					FTabManager::NewSplitter()
					->SetSizeCoefficient(0.2f)
					->SetOrientation(Orient_Vertical)
					->Split
					(
						// Right top - selection details panel & overrides
						FTabManager::NewStack()
						->SetHideTabWell(false)
						->SetSizeCoefficient(0.5f)
						->AddTab(FBlueprintEditorTabs::DetailsID, ETabState::OpenedTab)
						//->SetForegroundTab(FBlueprintEditorTabs::DetailsID)
					)
					->Split
					(
						// Right bottom Sequencer
						FTabManager::NewStack()
						->SetHideTabWell(false)
						->SetSizeCoefficient(0.5f)
						->AddTab(FSkillAssetEditor::SequencerAreaTabID, ETabState::OpenedTab)
						//->SetForegroundTab(FSkillAssetEditor::SequencerAreaTabID)
					)
				)
			)
		);
	
	ToolbarExtender = MakeShareable(new FExtender);

	//Add compilation buttons to tool bar
	 if (UToolMenu* Toolbar = InEditor->RegisterModeToolbarIfUnregistered(GetModeName()))
	 {
	 	
	 	InEditor->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
	 	InEditor->GetToolbarBuilder()->AddScriptingToolbar(Toolbar);
	 		
	 }
	
	
}

void SkillAssetEditorAPPMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	//get the actual ref of editor
	TSharedPtr<FBlueprintEditor> Editor = MyBlueprintEditor.Pin();

	//register tool bar tab
	Editor->RegisterToolbarTab(InTabManager.ToSharedRef());

	//Submit all tab factories to tab manager
	Editor->PushTabFactories(CoreTabFactories);
	Editor->PushTabFactories(BlueprintEditorTabFactories);
	Editor->PushTabFactories(SkillAssetTabFactories);
}

TSharedPtr<SKillAssetPriveiwScene>& SkillAssetEditorAPPMode::GetThePreviewSceneInsideTheMode()
{
	return PreviewTabFactory->GetFactoryTabBody()->GetSKAViewport()->GetSKAPreview();
}

