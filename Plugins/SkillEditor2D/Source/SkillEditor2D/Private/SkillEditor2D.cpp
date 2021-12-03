// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillEditor2D.h"

#include "AssetToolsModule.h"
#include"SkillEditorWindowStyle.h"
#include "FSkillEditorcommands.h"

#include "IAssetTools.h"
#include "IPluginManager.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "SkillAssetAction.h"
#include "SkillAssetEditor.h"
#include "SViewport.h"
#include "Slate/SceneViewport.h"
#include "Sequencer/Private/SSequencerTrackArea.h"


static const FName SkillEditor2DTabName("SkillEditor2D");
#define LOCTEXT_NAMESPACE "FSkillEditor2DModule"


ISkillAssetEditorModule_Base::ISkillAssetEditorModule_Base()
{
}

ISkillAssetEditorModule_Base::~ISkillAssetEditorModule_Base()
{
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

	// RealRenderingClient=MakeShareable(new FSkillEditorViewPortRenderingClient());
	//

	
	
}

void FSkillEditor2DModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	SkillEditorWindowStyle::ShutDown();
	FSkillEditorcommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SkillEditor2DTabName);


	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();
	// RealRenderingClient.Reset();
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
	
	// NewSkillAssetEditor.Get().RenderingClient=this->RealRenderingClient.Get();
	

	
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

IMPLEMENT_MODULE(FSkillEditor2DModule, SkillEditor2D)