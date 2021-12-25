// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillEditor2D.h"

#include "AssetToolsModule.h"

#include "EdGraphUtilities.h"
#include"SkillEditorWindowStyle.h"
#include "FSkillEditorcommands.h"

#include "IAssetTools.h"
#include "IPluginManager.h"
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
#include "SlateStyleRegistry.h"
#include "SViewport.h"
#include "USKAInstance.h"
#include "Engine/MemberReference.h"
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
	


	//Register graph nodes, pins , connection policy
	// FEdGraphUtilities::RegisterVisualNodeFactory(MakeShareable(new SKAGraphNodeFactory()));
	// FEdGraphUtilities::RegisterVisualPinFactory(MakeShareable(new SKAGraphPinFactory()));
	// FEdGraphUtilities::RegisterVisualPinConnectionFactory(MakeShareable(new SKAGraphPinConnectionFactory()));
	
	// Register widget blueprint compiler we do this no matter what.
	IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetCompilers().Add(this);

	FKismetCompilerContext::RegisterCompilerForBP(USkillAsset::StaticClass(), &FSkillEditor2DModule::GetCompilerForSKABP);
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-modul
	FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback(this,
		USKAInstance::StaticClass(),
		FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw
		(this, &FSkillEditor2DModule::onNewBlueprintCreated));
	SkillEditorWindowStyle::Initialize();
	SkillEditorWindowStyle::Reloadtextures();
	//register the command to the system
	FSkillEditorcommands::Register();
	//map the command from system click to action
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
	//action registration
	//asset registration!s
	AssetToolsModule=&FModuleManager::
	LoadModuleChecked<FAssetToolsModule>("AssetTools").
	Get();
	//
	SkillAsset2DAction=MakeShareable(new SkillAssetAction(SKACategory));
	AssetToolsModule->RegisterAssetTypeActions(SkillAsset2DAction.ToSharedRef());
	//Register new thumbnail
	StyleSet=MakeShareable(new FSlateStyleSet("SkillAssetStyle"));
	FString contentDir=IPluginManager::Get().FindPlugin("SkillEditor2D")->GetBaseDir();
	StyleSet->SetContentRoot(contentDir);

	FSlateImageBrush* thumbNailBrush=new FSlateImageBrush
	(StyleSet->RootToContentDir
		(TEXT
			("Resources/Icon128"), TEXT
			(".png")), FVector2D
			(128.f, 128.f));

	if(thumbNailBrush)
	{
		StyleSet->Set
		("ClassThumbnail.SkillAsset",
			thumbNailBrush);
	}
	if(StyleSet.IsValid())
		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);


	
	RegisterSettings();




	
	
	
	

	
	
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
	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(SkillAsset2DAction.ToSharedRef());
	}
	

	UnregisterSettings();

	

	
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
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