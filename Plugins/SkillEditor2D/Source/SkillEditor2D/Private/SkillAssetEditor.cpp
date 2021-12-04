// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditor.h"

#include "SceneViewport.h"
#include "SkillEditor2D.h"
#include "SViewport.h"


const FName SkillAssetEditorAppIdentifier = FName(TEXT("SkillAssetEditorApp"));
#define LOCTEXT_NAMESPACE "SkillAssetEditor"

const FName FSkillAssetEditor::ToolkitFName(TEXT("SkillAssetEditor"));
const FName FSkillAssetEditor::PropertiesTabId(TEXT("SkillAssetEditor_Properties"));

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
}

void FSkillAssetEditor::UnregisterTabSpawners
(const TSharedRef<FTabManager>& TabManager)
{
	// Unregister the tab manager from the asset editor toolkit
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	// Unregister our custom tab from the tab manager, making sure it is cleaned up when the editor gets destroyed
	TabManager->UnregisterTabSpawner(PropertiesTabId);
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
			FTabManager::NewSplitter()
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
}

FSkillAssetEditor::~FSkillAssetEditor()
{
	DetailsView.Reset();
	PropertiesTab.Reset();
	
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
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.Padding(2.0f)
			[
			//SNew(SSequencerTrackArea)
			SNew(SSplitter).Orientation(EOrientation::Orient_Horizontal)
			+SSplitter::Slot() //left area containing a view port and properties detail
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
			]
			+SSplitter::Slot()
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
				SNew(STextBlock).Text(FText::FromString("Graph"))
			]
			+SSplitter::Slot()
			[
			SNew(STextBlock).Text(FText::FromString("Sequencer Area"))
			]
		]
			]
			]
			
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


#undef LOCTEXT_NAMESPACE
