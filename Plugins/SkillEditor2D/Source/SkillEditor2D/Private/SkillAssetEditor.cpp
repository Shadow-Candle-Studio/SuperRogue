// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditor.h"

#include "BlueprintEditorUtils.h"
#include "FSkillEditorcommands.h"
#include "SceneViewport.h"
#include "SKAUEdGraphSchema.h"
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


void FSkillAssetEditor::RegisterTabSpawners
(const TSharedRef<FTabManager>& TabManager)
{
	WorkspaceMenuCategory=TabManager->AddLocalWorkspaceMenuCategory
	(LOCTEXT("WorkspaceMenu_SkillAssetEditor","Skill Asset Editor"));


	
	
	

	
	//register toolbar
	// const auto& LocalCategories = TabManager->GetLocalWorkspaceMenuRoot()->GetChildItems();
	// TSharedRef<FWorkspaceItem> ToolbarSpawnerCategory = LocalCategories.Num() > 0 ? LocalCategories[0] : TabManager->GetLocalWorkspaceMenuRoot();
	//
	// TabManager->RegisterTabSpawner( GetToolbarTabId(), FOnSpawnTab::CreateSP(this, &FAssetEditorToolkit::SpawnTab_Toolbar) )
	// 	.SetDisplayName( LOCTEXT("ToolbarTab", "Toolbar") )
	// 	.SetGroup( ToolbarSpawnerCategory )
	// 	.SetIcon( FSlateIcon(FEditorStyle::GetStyleSetName(), "Toolbar.Icon") );

	
}

void FSkillAssetEditor::UnregisterTabSpawners
(const TSharedRef<FTabManager>& TabManager)
{
	// Unregister the tab manager from the asset editor toolkit
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	// // Unregister our custom tab from the tab manager, making sure it is cleaned up when the editor gets destroyed
	// TabManager->UnregisterTabSpawner(PreviewTabId);
	// TabManager->UnregisterTabSpawner(GraphCanvasId);
	// TabManager->UnregisterTabSpawner(PropertiesPanelTabID);
	// TabManager->UnregisterTabSpawner(SequencerAreaTabID);
}

void FSkillAssetEditor::InitSkillAssetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillAsset* InSkillAsset)
{
	
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
	// Set this InCustomAsset as our editing asset
	SetSkillAsset(InSkillAsset);
	// Initialize our custom asset editor
	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost,
		SkillAssetEditorAppIdentifier,
		DummyLayout,
		bCreateDefaultStandaloneMenu,
		bCreateDefaultToolbar,
		(UObject*)InSkillAsset);
	DocumentManager=MakeShareable(new FDocumentTracker);
	DocumentManager->Initialize(SharedThis(this));
	DocumentManager->SetTabManager(TabManager.ToSharedRef());
	
	SKAEditorModeInuse=MakeShareable
		(new SkillAssetEditorAPPMode
			(SharedThis(this),
				SkillAssetEditorAPPMode::SKAModeID));
	//set the mode of this application and tabfactories
	AddApplicationMode(SkillAssetEditorAPPMode::SKAModeID,SKAEditorModeInuse.ToSharedRef());
	SetCurrentMode(SkillAssetEditorAPPMode::SKAModeID);

	

	SkillAssetExtcommands=MakeShareable(new FUICommandList);
	SkillAssetExtcommands->MapAction(FSkillEditorcommands::Get().Textfunc,
		FExecuteAction::CreateRaw(this,&FSkillAssetEditor::TextFuncOncliked),
		FCanExecuteAction());

	ExtendMenu();
	ExtendToolBar();
	
}

FSkillAssetEditor::~FSkillAssetEditor()
{
	;
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

void FSkillAssetEditor::SetCurrentMode(FName NewMode)
{
	ISkillAssetEditor::SetCurrentMode(NewMode);
}



void FSkillAssetEditor::SetSkillAsset(USkillAsset* InSkillAsset)
{
	SkillAsset=InSkillAsset;
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

void FSkillAssetEditor::InvokeSkillAssetEventBPGraphTab()
{
	bool bNewGraph = false;
	if (!EventGraph.IsValid())
	{
		bNewGraph = true;
		EventGraph = MakeShareable(FBlueprintEditorUtils::CreateNewGraph(
			(UObject*)GetSkillAsset(), 
			GraphCanvasId,
			USKAUEdGraph::StaticClass(), 
			USKAUEdGraphSchema::StaticClass()));
		
	}

	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(EventGraph.Get());
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, bNewGraph ? FDocumentTracker::OpenNewDocument :
		FDocumentTracker::RestorePreviousDocument);
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
	UE_LOG(LogTemp,Warning,L"Text Func clicked!")
}



USkillAsset* FSkillAssetEditor::GetSkillAsset()
{
	return SkillAsset;
}


#undef LOCTEXT_NAMESPACE
