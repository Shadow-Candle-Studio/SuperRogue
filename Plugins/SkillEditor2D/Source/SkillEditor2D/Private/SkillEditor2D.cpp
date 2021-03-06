// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillEditor2D.h"

#include "AssetToolsModule.h"

#include "EdGraphUtilities.h"
#include"SkillEditorWindowStyle.h"
#include "FSkillEditorcommands.h"

#include "IAssetTools.h"
#include "K2Node_Event.h"
#include "KismetEditorUtilities.h"
#include "LevelEditor.h"
#include "SKAFactory.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "SkillAssetAction.h"
#include "SkillAssetEditor.h"
#include "USKAInstance.h"
#include "Engine/MemberReference.h"
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
	//register the command to the system
	FSkillEditorcommands::Register();

	//window Style initialize
	SkillEditorWindowStyle::Initialize();
	SkillEditorWindowStyle::Reloadtextures();
	
	// Register widget blueprint compiler we do this no matter what.
	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetCompilers().Add(this);
	FKismetCompilerContext::RegisterCompilerForBP(USkillAsset::StaticClass(), &FSkillEditor2DModule::GetCompilerForSKABP);

	//map the command from system click to action
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(FSkillEditorcommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this,&FSkillEditor2DModule::PluginButtonClicked),
		FCanExecuteAction());
	
	// register callback when blueprint is created
	FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback(this,
		USKAInstance::StaticClass(),
		FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw
		(this, &FSkillEditor2DModule::onNewBlueprintCreated));
	
	//Register startup callback of the module to register the menus
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this,&FSkillEditor2DModule::RegisterMenus));

	//Register level editor window spawning tab
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SkillEditor2DTabName, FOnSpawnTab::CreateRaw(this, &FSkillEditor2DModule::OnSpawnPluginTab))
	.SetDisplayName(LOCTEXT("FSkillEditor2DTabTitle","SkillEditor2D"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	
	//allocate resources of the menu and toolbar extensibility manager
	MenuExtensibilityManager=MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager=MakeShareable(new FExtensibilityManager);

	
	//asset registration
	IAssetTools* AssetToolsModule=&FModuleManager::
	LoadModuleChecked<FAssetToolsModule>("AssetTools").
	Get();
	SkillAsset2DAction=MakeShareable(new SkillAssetAction(SKACategory));
	AssetToolsModule->RegisterAssetTypeActions(SkillAsset2DAction.ToSharedRef());

	
	RegisterSettings();




	
	
	
	

	
	
}

void FSkillEditor2DModule::ShutdownModule()
{
	
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	SkillEditorWindowStyle::ShutDown();
	FSkillEditorcommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SkillEditor2DTabName);
	
	// //Unregister all slate style resources
	// FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	// ensure(StyleSet.IsUnique());
	// StyleSet.Reset();
	
	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();
	
	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(SkillAsset2DAction.ToSharedRef());
	}
	

	UnregisterSettings();

	

	
	
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

void FSkillEditor2DModule::onNewBlueprintCreated(UBlueprint* InSkillAsset)
{
	if (ensure(InSkillAsset->UbergraphPages.Num() > 0))
	{
		UEdGraph* EventGraph = InSkillAsset->UbergraphPages[0];
		EventGraph->bEditable=true;
		UK2Node_Event* NewEventNode = nullptr;
		FMemberReference EventReference;
		EventReference.SetExternalMember(FName("SKAInstance"), USKAInstance::StaticClass());

		// Add the event
		NewEventNode = NewObject<UK2Node_Event>(EventGraph);
		NewEventNode->EventReference = EventReference;

		// add update event graph
		NewEventNode->bOverrideFunction = true;
		NewEventNode->CreateNewGuid();
		NewEventNode->PostPlacedNewNode();
		NewEventNode->SetFlags(RF_Transactional);
		NewEventNode->AllocateDefaultPins();
		NewEventNode->bCommentBubblePinned = true;
		NewEventNode->bCommentBubbleVisible = true;
		NewEventNode->NodePosY = 0;
		UEdGraphSchema_K2::SetNodeMetaData(NewEventNode, FNodeMetadata::DefaultGraphNode);

		EventGraph->AddNode(NewEventNode);
		NewEventNode->MakeAutomaticallyPlacedGhostNode();
		
	}
}

TSharedRef<ISkillAssetEditor> FSkillEditor2DModule::CreateSkillAssetBPEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillAsset* SKA)
{

	TSharedRef<FSkillAssetEditor > NewSkillAssetEditorEditor(new FSkillAssetEditor());
	NewSkillAssetEditorEditor->InitSkillAssetEditor(Mode,InitToolkitHost,SKA);
	UEdGraph* EventGraph = SKA->UbergraphPages[0];
	EventGraph->bEditable=true;
	NewSkillAssetEditorEditor->OpenDocument(EventGraph,FDocumentTracker::QuickNavigateCurrentDocument);
	return NewSkillAssetEditorEditor;
}


bool FSkillEditor2DModule::CanCompile(const UBlueprint* Blueprint)
{
	return Cast<USkillAsset>(Blueprint)!=nullptr;
}

void FSkillEditor2DModule::PreCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions)
{
	IBlueprintCompiler::PreCompile(Blueprint, CompileOptions);
}

void FSkillEditor2DModule::Compile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions,
	FCompilerResultsLog& Results)
{
	if (USkillAsset* SKA = CastChecked<USkillAsset>(Blueprint))
	{
		SKACompilerContext Compiler(SKA, Results, CompileOptions);
		Compiler.Compile();
		check(Compiler.NewClass);
	}
}

void FSkillEditor2DModule::PostCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions)
{
	IBlueprintCompiler::PostCompile(Blueprint, CompileOptions);
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