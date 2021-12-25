﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditorAPPMode.h"

#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "SkillAssetBPGraphTabSummoner.h"
#include "SkillAssetEditorSequenceTabSummoner.h"
#include "SkillAssetPropertyTabSummoner.h"
#include "SkillEditorPreviewSummoner.h"

const FName SkillAssetEditorAPPMode::SKAModeID(TEXT("SKAMode"));

SkillAssetEditorAPPMode::SkillAssetEditorAPPMode
(TSharedPtr<class FSkillAssetEditor> InEditor, FName InModeName):
FBlueprintEditorApplicationMode(InEditor, SkillAssetEditorAPPMode::SKAModeID, SkillAssetEditorAPPMode::GetLocalizedMode, false, false)
{
	AssetEditor=InEditor;
	// SkillAssetTabFactories.RegisterFactory
	// (MakeShareable(new SkillAssetPropertyTabSummoner(InEditor)));
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
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				// Top toolbar
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
						//	Left bottom - preview settings
						FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab(FSkillAssetEditor::PreviewTabId, ETabState::OpenedTab)
					)
					->Split
					(
					//	Left bottom - preview settings
					FTabManager::NewStack()
					->SetSizeCoefficient(0.5f)
					->AddTab(FBlueprintEditorTabs::MyBlueprintID, ETabState::OpenedTab)
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
						->SetForegroundTab(FBlueprintEditorTabs::DetailsID)
					)
					->Split
					(
						// Right bottom - Asset browser
						FTabManager::NewStack()
						->SetHideTabWell(false)
						->SetSizeCoefficient(0.5f)
						->AddTab(FSkillAssetEditor::SequencerAreaTabID, ETabState::OpenedTab)
						->SetForegroundTab(FSkillAssetEditor::SequencerAreaTabID)
					)
				)
			)
		);
	
	ToolbarExtender = MakeShareable(new FExtender);
	
	if (UToolMenu* Toolbar = InEditor->RegisterModeToolbarIfUnregistered(GetModeName()))
	{
		if(InEditor->GetToolbarBuilder().IsValid())
		{
			InEditor->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
			InEditor->GetToolbarBuilder()->AddScriptingToolbar(Toolbar);
		}
			
	}
	
	
}

void SkillAssetEditorAPPMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FSkillAssetEditor> Editor = AssetEditor.Pin();

	// Tool bar tab
	
	Editor->RegisterToolbarTab(InTabManager.ToSharedRef());




	Editor->PushTabFactories(CoreTabFactories);
	Editor->PushTabFactories(BlueprintEditorTabFactories);
	//Register other factories
	Editor->PushTabFactories(SkillAssetTabFactories);

	// Graph tab
	Editor->DocumentManager->RegisterDocumentFactory(MakeShareable(new SkillAssetBPGraphTabSummoner(Editor)));

	FBlueprintEditorApplicationMode::RegisterTabFactories(InTabManager);
	//FApplicationMode::RegisterTabFactories(InTabManager);
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


void SkillAssetEditorAPPMode::PostActivateMode()
{
	
}

SkillAssetEditorAPPMode::~SkillAssetEditorAPPMode()
{
}
