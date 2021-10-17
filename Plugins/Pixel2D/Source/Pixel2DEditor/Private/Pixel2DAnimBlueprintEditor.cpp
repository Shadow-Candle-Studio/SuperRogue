// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimBlueprintEditor.h"

#include "BlueprintEditor.h"
#include "SBlueprintEditorToolbar.h"
#include "Pixel2DAnimBlueprintEditorMode.h"
#include "Pixel2DEditorModule.h"

#include "BlueprintEditorTabs.h"
#include "Pixel2DAnimGraphNode_AssetSprite.h"
#include "PaperFlipbook.h"
#include "Pixel2DAnimNotifyEditor.h"

const FName Pixel2DAnimBlueprintEditorAppName(TEXT("SpriteAnimationBlueprintEditorApp"));
const FName FPixel2DAnimBlueprintEditorModes::AnimationBlueprintEditorMode("GraphName"); // For backwards compatibility we keep the old mode name here


namespace Pixel2DAnimationBlueprintEditorTabs
{
	const FName AssetBrowserTab(TEXT("FlipbookBrowser"));
};

class FBlueprintEditorToolbar;

void FPixel2DAnimBlueprintEditor::InitSpriteAnimBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UPixel2DAnimBlueprint* InSpriteAnimBlueprint)
{
	if (!Toolbar.IsValid())
	{
		Toolbar = MakeShareable(new FBlueprintEditorToolbar(SharedThis(this)));
	}

	// Build up a list of objects being edited in this asset editor
	TArray<UObject*> ObjectsBeingEdited;
	ObjectsBeingEdited.Add(InSpriteAnimBlueprint);

	CreateDefaultCommands();

	// Initialize the asset editor and spawn tabs
	const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode, InitToolkitHost, Pixel2DAnimBlueprintEditorAppName, DummyLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsBeingEdited);

	TArray<UBlueprint*> AnimBlueprints;
	AnimBlueprints.Add(InSpriteAnimBlueprint);

	CommonInitialization(AnimBlueprints, false);

	AddApplicationMode(
		FPixel2DAnimBlueprintEditorModes::AnimationBlueprintEditorMode,
		MakeShareable(new FPixel2DAnimBlueprintEditorMode(SharedThis(this))));

	ExtendMenu();
	ExtendToolbar();
	RegenerateMenusAndToolbars();

	// Activate the initial mode (which will populate with a real layout)
	SetCurrentMode(FPixel2DAnimBlueprintEditorModes::AnimationBlueprintEditorMode);

	// Post-layout initialization
	PostLayoutBlueprintEditorInitialization();

	// register the AnimNotifyEditor
	AnimNotifyEditor = MakeShareable(new FPixel2DAnimNotifyEditor());
	// Make sure the tab it's not registered for a old instance
	FGlobalTabmanager::Get()->UnregisterTabSpawner(FPixel2DAnimNotifyEditor::Pixel2DAnimNotifyEditorTabName);
	AnimNotifyEditorTabSpawner = &FGlobalTabmanager::Get()->RegisterTabSpawner(FPixel2DAnimNotifyEditor::Pixel2DAnimNotifyEditorTabName, FOnSpawnTab::CreateRaw(AnimNotifyEditor.Get(), &FPixel2DAnimNotifyEditor::OnSpawnPluginTab));
	AnimNotifyEditorTabSpawner->SetMenuType(ETabSpawnerMenuType::Hidden);
}

UObject * FPixel2DAnimBlueprintEditor::HandleGetObject()
{
	return nullptr;
}

void FPixel2DAnimBlueprintEditor::HandleOpenNewAsset(UObject * InNewAsset)
{}

UBlueprint* FPixel2DAnimBlueprintEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<UPixel2DAnimBlueprint>()) { return (UBlueprint*)EditingObjs[i]; }
	}
	return nullptr;
}


UPixel2DAnimBlueprint* FPixel2DAnimBlueprintEditor::GetSpriteAnimBlueprint() const
{
	return Cast<UPixel2DAnimBlueprint>(GetBlueprintObj());
}

void FPixel2DAnimBlueprintEditor::ExtendMenu()
{
	if (MenuExtender.IsValid())
	{
		RemoveMenuExtender(MenuExtender);
		MenuExtender.Reset();
	}

	MenuExtender = MakeShareable(new FExtender);
	AddMenuExtender(MenuExtender);
}

void FPixel2DAnimBlueprintEditor::ExtendToolbar()
{
	// If the ToolbarExtender is valid, remove it before rebuilding it
	if (ToolbarExtender.IsValid())
	{
		RemoveToolbarExtender(ToolbarExtender);
		ToolbarExtender.Reset();
	}

	ToolbarExtender = MakeShareable(new FExtender);

	AddToolbarExtender(ToolbarExtender);
}

void FPixel2DAnimBlueprintEditor::OnOpenRelatedAsset()
{
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	EToolkitMode::Type Mode = EToolkitMode::Standalone;
	if (SelectedNodes.Num() > 0)
	{
		for (auto NodeIter = SelectedNodes.CreateIterator(); NodeIter; ++NodeIter)
		{
			if (UPixel2DAnimGraphNode_AssetSprite* Node = Cast<UPixel2DAnimGraphNode_AssetSprite>(*NodeIter))
			{
				UPaperFlipbook* AnimAsset = Node->Node.AssetFlipbook;
				if (AnimAsset)
				{
				  //  FAssetEditorManager::Get().OpenEditorForAsset(Node, Mode);
					AnimNotifyEditor->InitNotifyEditor(Node);
					FGlobalTabmanager::Get()->TryInvokeTab(FPixel2DAnimNotifyEditor::Pixel2DAnimNotifyEditorTabName);

				}
			}
		}
	}
}
