// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillEditor2D.h"

#include "AssetToolsModule.h"
#include"SkillEditorWindowStyle.h"
#include "FSkillEditorcommands.h"
#include "IAssetTools.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "SkillAssetAction.h"
const FName SkillAssetEditorAppIdentifier = FName(TEXT("SkillAssetEditorApp"));
static const FName SkillEditor2DTabName("SkillEditor2D");
#define LOCTEXT_NAMESPACE "FSkillEditor2DModule"


ISkillAssetEditorModule_Base::ISkillAssetEditorModule_Base()
{
}

ISkillAssetEditorModule_Base::~ISkillAssetEditorModule_Base()
{
	;
}

void FSkillEditor2DModule::StartupModule()
{
	

	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-modul
	SkillEditorWindowStyle::Initialize();
	SkillEditorWindowStyle::Reloadtextures();
	FSkillEditorcommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(FSkillEditorcommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this,&FSkillEditor2DModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,&FSkillEditor2DModule::RegisterMenus));
	UE_LOG(LogTemp,Warning,L"Skill editor 2D started1")
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SkillEditor2DTabName, FOnSpawnTab::CreateRaw(this, &FSkillEditor2DModule::OnSpawnPluginTab))
	.SetDisplayName(LOCTEXT("FSkillEditor2DTabTitle","SkillEditor2D"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	MenuExtensibilityManager=MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager=MakeShareable(new FExtensibilityManager);




	
	
}

void FSkillEditor2DModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	SkillEditorWindowStyle::ShutDown();
	FSkillEditorcommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner
	(SkillEditor2DTabName);


	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();
}
TSharedRef<SDockTab> FSkillEditor2DModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText=FText::Format(
	LOCTEXT("WindowWidgetText","Add code to {0} in {1} to override this window's contents"),
	FText::FromString(TEXT("FSkillEditor2DModule::OnSpawnPluginTab")),
	FText::FromString(TEXT("SkillEditor2D.cpp"))
		);
	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(WidgetText)

		]

	];
}

void FSkillEditor2DModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(SkillEditor2DTabName);
}

TSharedRef<ISkillAssetEditor> FSkillEditor2DModule::CreateCustomAssetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillAsset* CustomAsset)
{
	TSharedRef<FSkillAssetEditor>NewSkillAssetEditor(new FSkillAssetEditor());
	NewSkillAssetEditor->InitSkillAssetEditor(Mode,InitToolkitHost,CustomAsset);
	return NewSkillAssetEditor;
}

void FSkillEditor2DModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	{
		UToolMenu* Menu=UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section=Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FSkillEditorcommands::Get().OpenPluginWindow,PluginCommands);
		}
	}
	{
		UToolMenu* ToolbarMenu=UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section=ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry=Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSkillEditorcommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}




#undef LOCTEXT_NAMESPACE



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

TSharedRef<SDockTab> FSkillAssetEditor::SpawnPropertiesTab(const FSpawnTabArgs& Args)
{
	// Make sure we have the correct tab id
	check(Args.GetTabId() == PropertiesTabId);
    UE_LOG(LogTemp, Warning,L"Properties SSSSS launched!")
	// Return a new slate dockable tab that contains our details view
	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush
			("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("GenericDetailsTitle", "Details"))
		.TabColorScale(GetTabColorScale())
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Top)
			.Padding(2.0f)
			[
			SNew(STextBlock).Text(FText::FromString(L"233333"))
			]
			+SHorizontalBox::Slot()
			[
			DetailsView.ToSharedRef()
			]
			
			
			
		];

	// SNew(SDockTab)
	// .TabRole(ETabRole::NomadTab)
	// [
	// 	SNew(SBox)
	// 	.HAlign(HAlign_Center)
	// 	.VAlign(VAlign_Center)
	// 	[
	// 		SNew(STextBlock)
	// 		.Text(WidgetText)
	//
	// 	]
	//
	// ];
}


#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FSkillEditor2DModule, SkillEditor2D)