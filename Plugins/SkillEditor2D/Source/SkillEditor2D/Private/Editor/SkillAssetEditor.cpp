// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditor.h"

#include "BlueprintEditorUtils.h"
#include "FSkillEditorcommands.h"
#include "SBlueprintEditorToolbar.h"
#include "SkillAsset.h"
#include "SkillAssetEditorAPPMode.h"
#include "SkillEditor2D.h"
#include "ToolMenus.h"


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



void FSkillAssetEditor::InitSkillAssetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillAsset* InSkillAsset)
{
	//Map the click message to specific function
	SkillAssetExtcommands=MakeShareable(new FUICommandList);
	SkillAssetExtcommands->MapAction(FSkillEditorcommands::Get().Textfunc,
		FExecuteAction::CreateRaw(this,&FSkillAssetEditor::TextFuncOncliked),
		FCanExecuteAction());
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





void FSkillAssetEditor::SetSkillAsset(USkillAsset* InSkillAsset)
{
	SkillAsset=InSkillAsset;
}


void FSkillAssetEditor::ExtendMenu()
{
	if (MenuExtender.IsValid())
	{
		RemoveMenuExtender(MenuExtender);
		MenuExtender.Reset();
	}
	
	MenuExtender = MakeShareable(new FExtender);
	AddMenuExtender(MenuExtender);
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
	// If the ToolbarExtender is valid, remove it before rebuilding it
	if (ToolbarExtender.IsValid())
	{
		RemoveToolbarExtender(ToolbarExtender);
		ToolbarExtender.Reset();
	}

	ToolbarExtender = MakeShareable(new FExtender);

	AddToolbarExtender(ToolbarExtender);
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

void FSkillAssetEditor::FillToolbar(FToolBarBuilder& ToolBarbuilder)
{
	ToolBarbuilder.BeginSection("ExtendToolbarItem");
	{
		const FText NewNodeLabel = LOCTEXT("NewNode_Label", "New");
		const FText NewNodeTooltip = LOCTEXT("NewNode_ToolTip", "Create a new node");
		const FSlateIcon NewNodeIcon = FSlateIcon(TEXT("EditorStyle"), "SessionConsole.Clear");

		ToolBarbuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateSP(this, &FSkillAssetEditor::TextFuncOncliked),
				FCanExecuteAction::CreateSP(this, &FSkillAssetEditor::canExecuteBar),
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
	UE_LOG(LogTemp,Warning,L"Textfunconcliced!")
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
