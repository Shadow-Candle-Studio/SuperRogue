// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditor.h"

#include "FSkillEditorcommands.h"
#include "SceneViewport.h"
#include "SkillAssetEditorAPPMode.h"
#include "SkillEditor2D.h"
#include "SViewport.h"
#include "ToolMenus.h"


const FName SkillAssetEditorAppIdentifier = FName(TEXT("SkillAssetEditorApp"));
#define LOCTEXT_NAMESPACE "SkillAssetEditor"

const FName FSkillAssetEditor::ToolkitFName
(TEXT("SkillAssetEditor"));
const FName FSkillAssetEditor::PropertiesTabId
(TEXT("SkillAssetEditor_Properties"));
const FName FSkillAssetEditor::GraphCanvasId
(TEXT("SkillAssetEditor_BPGraph"));

void FSkillAssetEditor::RegisterTabSpawners
(const TSharedRef<FTabManager>& TabManager)
{
	WorkspaceMenuCategory=TabManager->AddLocalWorkspaceMenuCategory
	(LOCTEXT("WorkspaceMenu_SkillAssetEditor","Skill Asset Editor"));


	
	FAssetEditorToolkit::RegisterTabSpawners(TabManager);

	TabManager->RegisterTabSpawner(PropertiesTabId,
		FOnSpawnTab::CreateSP(this,
			&FSkillAssetEditor::SpawnPropertiesTab))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
			"LevelEditor.Tabs.Details"));

	TabManager->RegisterTabSpawner(GraphCanvasId,
		FOnSpawnTab::CreateSP(this,
		&FSkillAssetEditor::SpawnBPGraphTab))
	.SetDisplayName(LOCTEXT("GraphCanvasTab", "Graph"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(),
			"GraphEditor.EventGraph_16x"));
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

	// Unregister our custom tab from the tab manager, making sure it is cleaned up when the editor gets destroyed
	TabManager->UnregisterTabSpawner(PropertiesTabId);
	TabManager->UnregisterTabSpawner(GraphCanvasId);
}

void FSkillAssetEditor::InitSkillAssetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillAsset* InSkillAsset)
{
	// Cache some values that will be used for our details view arguments
	const bool bIsUpdatable = false;
	const bool bAllowFavorites = true;
	const bool bIsLockable = false;

	// Set this InCustomAsset as our editing asset
	SetSkillAsset(InSkillAsset);

	// Retrieve the property editor module and assign properties to DetailsView
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(bIsUpdatable, bIsLockable, true, FDetailsViewArgs::ObjectsUseNameArea, false);
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// Create the layout of our custom asset editor
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("SkillAssetEditor_Layout_v1")
	->AddArea
	(
		// Create a vertical area and spawn the toolbar
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(0.1f)
			->SetHideTabWell(true)
			->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
		)
		->Split
		(
			// Split the tab and pass the tab id to the tab spawner
			FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
			->Split
			(
			FTabManager::NewStack()
			//->SetSizeCoefficient(0.7f)
			->AddTab(GraphCanvasId,ETabState::OpenedTab)
			)
			->Split
			(
			FTabManager::NewStack()
			->AddTab(PropertiesTabId, ETabState::OpenedTab)
			
			)
		)
	);
 
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;

	// Initialize our custom asset editor
	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost,
		SkillAssetEditorAppIdentifier,
		StandaloneDefaultLayout,
		bCreateDefaultStandaloneMenu,
		bCreateDefaultToolbar,
		(UObject*)InSkillAsset);
    
	// Set the asset we are editing in the details view
	if (DetailsView.IsValid())
	{
		DetailsView->SetObject((UObject*)InSkillAsset);
	}


	SkillAssetExtcommands=MakeShareable(new FUICommandList);
	SkillAssetExtcommands->MapAction(FSkillEditorcommands::Get().Textfunc,
		FExecuteAction::CreateRaw(this,&FSkillAssetEditor::TextFuncOncliked),
		FCanExecuteAction());

	ExtendMenu();
	ExtendToolBar();
	
}

FSkillAssetEditor::~FSkillAssetEditor()
{
	DetailsView.Reset();
	//PropertiesTab.Reset();
	
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

USkillAsset* FSkillAssetEditor::GetSkillAsset()
{
	return SkillAsset;
}

void FSkillAssetEditor::SetSkillAsset(USkillAsset* InSkillAsset)
{
	SkillAsset=InSkillAsset;
}

TSharedRef<SWidget> FSkillAssetEditor::SpawnPreview()
{
	SAssignNew(SkillAssetTabBody,SkillEditorPreviewTabBody);
	return SkillAssetTabBody.ToSharedRef();
}


void FSkillAssetEditor::ExtendMenu()
{
	// struct Local
	// {
	// 	static void FillEditMenu(FMenuBuilder& MenuBuilder)
	// 	{
	// 		MenuBuilder.BeginSection("EditSearch", LOCTEXT("EditMenu_SearchHeading", "Search"));
	// 		{
	// 			MenuBuilder.AddMenuEntry(FSkillEditorcommands::Get().Textfunc);
	// 		}
	// 		MenuBuilder.EndSection();
	// 	}
	// };
	//
	// TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
	//
	// // Extend the Edit menu
	// MenuExtender->AddMenuExtension(
	// 	"EditHistory",
	// 	EExtensionHook::After,
	// 	GetToolkitCommands(),
	// 	FMenuExtensionDelegate::CreateStatic(&Local::FillEditMenu));
	//
	// AddMenuExtender(MenuExtender);
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
	// TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	// ToolbarExtender->AddToolBarExtension
	// ("Asset",
	// 	EExtensionHook::After,
	// 	this->SkillAssetExtcommands,
	// 	FToolBarExtensionDelegate::CreateSP
	// 	(this,
	// 		&FSkillAssetEditor::FillToolbar));
	// this->AddToolbarExtender(ToolbarExtender);
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

TSharedRef<SDockTab> FSkillAssetEditor::SpawnPropertiesTab(const FSpawnTabArgs& Args)
{
	// Make sure we have the correct tab id
	check(Args.GetTabId() == PropertiesTabId);
   
	// Return a new slate dockable tab that contains our details view
	TSharedRef<SDockTab> Tab=SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush
			("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("GenericDetailsTitle", "Details"))
		.TabColorScale(GetTabColorScale())
		[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.Padding(2.0f)
		[
			SNew(SSplitter).Orientation(EOrientation::Orient_Vertical)
			+SSplitter::Slot()
			[
				SNew(SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
				//SAssignNew(ViewportWidget, SViewport)
				SpawnPreview()
				]
			]
			+SSplitter::Slot()
			[
			DetailsView.ToSharedRef()
			]
		]
			// SNew(SHorizontalBox)
			// +SHorizontalBox::Slot()
			// .VAlign(VAlign_Fill)
			// .HAlign(HAlign_Fill)
			// .Padding(2.0f)
			
			//SNew(SSequencerTrackArea)
			// SNew(SSplitter).Orientation(EOrientation::Orient_Horizontal)
			// +SSplitter::Slot() //left area containing a view port and properties detail
			// [
			//
			// ]
		// 	+SSplitter::Slot()
		// 	[
		// 	SNew(SVerticalBox)
		// +SVerticalBox::Slot()
		// .VAlign(VAlign_Fill)
		// .HAlign(HAlign_Fill)
		// .Padding(2.0f)
		// [
		// 	SNew(SSplitter).Orientation(EOrientation::Orient_Vertical)
		// 	+SSplitter::Slot()
		// 	[
		// 		SNew(STextBlock).Text(FText::FromString("Graph"))
		// 	]
		// 	+SSplitter::Slot()
		// 	[
		// 	SNew(STextBlock).Text(FText::FromString("Sequencer Area"))
		// 	]
		// ]
		// 	]
			//]
			
		];

	// auto& mod=FModuleManager::GetModuleChecked<FSkillEditor2DModule>("SkillEditor2D").RealRenderingClient;
	// mod=MakeShareable(new FSkillEditorViewPortRenderingClient());
	// if(mod.IsValid())
	// {
	// 	Viewport=MakeShareable
 //   (new FSceneViewport(mod.Get(),ViewportWidget));
	// 	ViewportWidget->SetViewportInterface(Viewport.ToSharedRef());
	// }
	// else
	// {
	// 	//mod=MakeShareable(new FSkillEditorViewPortRenderingClient());
	// 	UE_LOG(LogTemp,Warning,L"Rendering client in module is invalid")
	// }

	
return Tab;
}

TSharedRef<SDockTab> FSkillAssetEditor::SpawnBPGraphTab(const FSpawnTabArgs& Args)
{
	// Make sure we have the correct tab id
	check(Args.GetTabId() == GraphCanvasId);
	TSharedRef<SDockTab> Tab=SNew(SDockTab)
		.Label(LOCTEXT("GraphCanvasTitle", "Events"))
		.TabColorScale(GetTabColorScale())
	[
	SNew(SVerticalBox)
    +SVerticalBox::Slot()
    .VAlign(VAlign_Fill)
    .HAlign(HAlign_Fill)
    .Padding(2.0f)
[
	SNew(SSplitter).Orientation(EOrientation::Orient_Vertical)
	+SSplitter::Slot()
	[
		SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(STextBlock).Text(FText::FromString("GraphArea"))
		//SAssignNew(ViewportWidget, SViewport)
		//SpawnPreview()
		]
	]
	+SSplitter::Slot()
	[
		SNew(STextBlock).Text(FText::FromString("Sequence Area"))
	//DetailsView.ToSharedRef()
	]
]
		
	];
	
	return Tab;
}


#undef LOCTEXT_NAMESPACE
