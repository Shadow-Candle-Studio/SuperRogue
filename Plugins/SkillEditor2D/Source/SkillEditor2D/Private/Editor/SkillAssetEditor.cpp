// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditor.h"

#include "BlueprintEditorUtils.h"
#include "FSkillEditorcommands.h"
#include "SBlueprintEditorToolbar.h"
#include "SceneViewport.h"

#include "SkillAsset.h"
#include "SkillAssetEditorAPPMode.h"
#include "SkillEditor2D.h"
#include "SViewport.h"
#include "ToolMenus.h"
#include "WorkflowUObjectDocuments.h"


const FName SkillAssetEditorAppIdentifier = FName(TEXT("SkillAssetEditorApp"));
#define LOCTEXT_NAMESPACE "SkillAssetEditor"

const FName FSkillAssetEditor::ToolkitFName
(TEXT("SkillAssetEditor"));
const FName FSkillAssetEditor::PreviewTabId
(TEXT("SkillAssetEditor_Preview"));
const FName FSkillAssetEditor::GraphCanvasId
(TEXT("SkillAssetEditor_BPGraph"));
const FName FSkillAssetEditor::PropertiesPanelTabID
(TEXT("SkillAssetEditor_Properties"));
const FName FSkillAssetEditor::SequencerAreaTabID
(TEXT("SkillAssetEditor_SequencerArea"));


UBlueprint* FSkillAssetEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<USkillAsset>()) { return (UBlueprint*)EditingObjs[i]; }
	}
	return nullptr;
}

void FSkillAssetEditor::RegisterTabSpawners
(const TSharedRef<FTabManager>& TabManager)
{
	//add new workspace menu category
	WorkspaceMenuCategory=TabManager->AddLocalWorkspaceMenuCategory
	(LOCTEXT("WorkspaceMenu_SkillAssetEditor","Skill Asset Editor"));
}

void FSkillAssetEditor::UnregisterTabSpawners
(const TSharedRef<FTabManager>& TabManager)
{
	// Unregister the tab manager from the asset editor toolkit
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	
}

void FSkillAssetEditor::InitSkillAssetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillAsset* InSkillAsset)
{
	if (!Toolbar.IsValid())
	{
		Toolbar = MakeShareable(new FBlueprintEditorToolbar(SharedThis(this)));
	}
	
	// Build up a list of objects being edited in this asset editor
	TArray<UObject*> ObjectsBeingEdited;
	ObjectsBeingEdited.Add(InSkillAsset);

	CreateDefaultCommands();

	// Initialize the asset editor and spawn tabs
	const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode,
		InitToolkitHost,
		SkillAssetEditorAppIdentifier,
		DummyLayout,
		bCreateDefaultStandaloneMenu,
		bCreateDefaultToolbar,
		ObjectsBeingEdited);
	
	
	TArray<UBlueprint*> Blueprints;
	Blueprints.Add(InSkillAsset);

	CommonInitialization(Blueprints, false);

	AddApplicationMode(
		SkillAssetEditorAPPMode::SKAModeID,
		MakeShareable(new SkillAssetEditorAPPMode(SharedThis(this))));

	ExtendMenu();
	ExtendToolBar();
	RegenerateMenusAndToolbars();

	// Activate the initial mode (which will populate with a real layout)
	SetCurrentMode(SkillAssetEditorAPPMode::SKAModeID);

	// Post-layout initialization
	PostLayoutBlueprintEditorInitialization();

	

	// SkillAssetExtcommands=MakeShareable(new FUICommandList);
	// SkillAssetExtcommands->MapAction(FSkillEditorcommands::Get().Textfunc,
	// 	FExecuteAction::CreateRaw(this,&FSkillAssetEditor::TextFuncOncliked),
	// 	FCanExecuteAction());


	
}

FSkillAssetEditor::~FSkillAssetEditor()
{
	DocumentManager.Reset();
	//EventGraph.Reset();
}

FName FSkillAssetEditor::GetToolkitFName() const
{
	return ToolkitFName;
}

FText FSkillAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Skill Asset Editor");
}

FText FSkillAssetEditor::GetToolkitName() const
{
	return FText::FromName(GetToolkitFName());
}

FText FSkillAssetEditor::GetToolkitToolTipText() const
{
	return LOCTEXT("ToolTip", "Skill Asset Editor");
}

FString FSkillAssetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Skill Asset Editor").ToString();
}

FLinearColor FSkillAssetEditor::GetWorldCentricTabColorScale() const
{
	return FColor::Black;
}

bool FSkillAssetEditor::IsPrimaryEditor() const
{
	return true;
}

// void FSkillAssetEditor::SetCurrentMode(FName NewMode)
// {
// 	
// }



void FSkillAssetEditor::SetSkillAsset(USkillAsset* InSkillAsset)
{
	SkillAsset=InSkillAsset;
}

void FSkillAssetEditor::OnOpenRelatedAsset()
{
	ISkillAssetEditor::OnOpenRelatedAsset();
}

void FSkillAssetEditor::ExtendMenu()
{
	
    // example of how to register an command
    FString WindowsubMenuName=this->GetToolMenuName().ToString();
	WindowsubMenuName+=".Window";
	UToolMenu* Menu=UToolMenus::Get()->ExtendMenu(FName(*WindowsubMenuName));
	{
		
		FToolMenuSection& Section=Menu->FindOrAddSection("General");
		Section.AddMenuEntryWithCommandList(FSkillEditorcommands::Get().Textfunc,SkillAssetExtcommands);
		Section.AddSubMenu("New Sub Menu", FText::FromString("???"), FText::FromString("???"),
					   FNewToolMenuChoice(
						   FNewMenuDelegate::CreateRaw(
							   this, &FSkillAssetEditor::FillsubMenu)));
	}
}

void FSkillAssetEditor::ExtendToolBar()
{
	
	FName ToolBarName;
	GetToolMenuToolbarName(ToolBarName);
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(ToolBarName);
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Asset");
		{
			FToolMenuEntry& Entry = Section.AddEntry(
				FToolMenuEntry::InitToolBarButton(FSkillEditorcommands::Get().Textfunc));
			Entry.SetCommandList(FSkillAssetEditor::SkillAssetExtcommands);
		}
	}
	
}

void FSkillAssetEditor::CreateNewNode()
{
	;
}

bool FSkillAssetEditor::CanCreateNewNode()
{
	return true;
}

void FSkillAssetEditor::RegisterToolbarTab(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	
}


void FSkillAssetEditor::FillToolbar(FToolBarBuilder& ToolBarbuilder)
{
	ToolBarbuilder.BeginSection("ExtendToolbarItem");
	{
		const FText NewNodeLabel = LOCTEXT("NewNode_Label", "New");
		const FText NewNodeTooltip = LOCTEXT("NewNode_ToolTip", "Create a new node");
		const FSlateIcon NewNodeIcon = FSlateIcon(TEXT("EditorStyle"), "SessionConsole.Clear");

		ToolBarbuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateSP(this, &FSkillAssetEditor::CreateNewNode),
				FCanExecuteAction::CreateSP(this, &FSkillAssetEditor::CanCreateNewNode),
				FIsActionChecked()
			),
			NAME_None,
			NewNodeLabel,
			NewNodeTooltip,
			NewNodeIcon
		);
	}
	ToolBarbuilder.EndSection();
}
void FSkillAssetEditor::FillsubMenu(FMenuBuilder& Menubuilder)
{
	{
		// Create the Submenu Entries
 
		Menubuilder.AddMenuEntry(
			FSkillEditorcommands::Get().Textfunc, NAME_None,
			FText::FromString("Menu Entry 1"),
			FText::FromString("Menu Entry 1 Tooltip"),
			FSlateIcon()
		);
	}
}
void FSkillAssetEditor::TextFuncOncliked()
{
	//InvokeSkillAssetEventBPGraphTab();
}



USkillAsset* FSkillAssetEditor::GetSkillAsset()
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<USkillAsset>()) { return (USkillAsset*)EditingObjs[i]; }
	}
	return nullptr;
}


#undef LOCTEXT_NAMESPACE
