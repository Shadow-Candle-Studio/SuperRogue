// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimBlueprintEditorMode.h"

#include "BlueprintEditorTabs.h"
#include "Pixel2DEditorModule.h"
#include "SBlueprintEditorToolbar.h"

FPixel2DAnimBlueprintEditorMode::FPixel2DAnimBlueprintEditorMode(const TSharedRef<FPixel2DAnimBlueprintEditor>& InAnimationBlueprintEditor)
	: FBlueprintEditorApplicationMode(
		InAnimationBlueprintEditor,
		FPixel2DAnimBlueprintEditorModes::AnimationBlueprintEditorMode,
		FPixel2DAnimBlueprintEditorModes::GetLocalizedMode,
		false,
		false)
{
	AnimBlueprintPtr = CastChecked<UPixel2DAnimBlueprint>(InAnimationBlueprintEditor->GetBlueprintObj());

	TabLayout = FTabManager::NewLayout("11111111Standalone_SpriteAnimationBlueprintEditMode_Layout_v0.0")
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
				->AddTab(InAnimationBlueprintEditor->GetToolbarTabId(), ETabState::OpenedTab)
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
						->AddTab(Pixel2DAnimationBlueprintEditorTabs::AssetBrowserTab, ETabState::OpenedTab)
						->SetForegroundTab(Pixel2DAnimationBlueprintEditorTabs::AssetBrowserTab)
					)
				)
			)
		);

	IPixel2DAnimEditorModule& Pixel2DEditorModule = FModuleManager::LoadModuleChecked<IPixel2DAnimEditorModule>("Pixel2DEditor");
	TabFactories.RegisterFactory(Pixel2DEditorModule.CreateAnimationAssetBrowserTabFactory(InAnimationBlueprintEditor, true));

	ToolbarExtender = MakeShareable(new FExtender);
	if (UToolMenu* Toolbar = InAnimationBlueprintEditor->RegisterModeToolbarIfUnregistered(GetModeName()))
	{
		InAnimationBlueprintEditor->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
		InAnimationBlueprintEditor->GetToolbarBuilder()->AddScriptingToolbar(Toolbar);
	}
}

void FPixel2DAnimBlueprintEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FBlueprintEditor> BP = MyBlueprintEditor.Pin();

	BP->RegisterToolbarTab(InTabManager.ToSharedRef());

	// Mode-specific setup
	BP->PushTabFactories(CoreTabFactories);
	BP->PushTabFactories(BlueprintEditorTabFactories);
	BP->PushTabFactories(TabFactories);
}
