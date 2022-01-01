// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetToolsModule.h"
#include "ISettingsModule.h"
#include "KismetCompilerModule.h"
#include "LevelEditor.h"
#include "SKACompilerContext.h"
#include "SkillAsset2DRuntimeSettings.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

class FSlateStyleSet;
class IAssetTypeActions;
class IAssetTools;
class ISkillAssetEditor;
class FToolBarBuilder;
class FMenuBuilder;
class USkillAsset;



#define LOCTEXT_NAMESPACE "SkillAsset2D_Module"

class SKILLEDITOR2D_API ISkillAssetEditorModule_Base:
public IModuleInterface,
public IHasMenuExtensibility,
public IHasToolBarExtensibility
{
public:
	virtual TSharedRef<ISkillAssetEditor> CreateSkillAssetBPEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost,USkillAsset* Blueprint) = 0;
	ISkillAssetEditorModule_Base();
	~ISkillAssetEditorModule_Base();
};






class FSkillEditor2DModule : public ISkillAssetEditorModule_Base, public IBlueprintCompiler
{
public:
	
	
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void PluginButtonClicked();
	virtual void onNewBlueprintCreated(UBlueprint* InSkillAsset);
	virtual TSharedRef<ISkillAssetEditor> CreateSkillAssetBPEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost,USkillAsset* Blueprint) override;
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override
	{
		return  MenuExtensibilityManager;
	}
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override
	{
		return ToolBarExtensibilityManager;
	}
	virtual bool CanCompile(const UBlueprint* Blueprint) override;
	virtual void PreCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions) override;
	virtual void Compile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions, FCompilerResultsLog& Results) override;
	virtual void PostCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions) override;
	static TSharedPtr<FKismetCompilerContext> GetCompilerForSKABP(UBlueprint* BP, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompileOptions)
	{
		return TSharedPtr<FKismetCompilerContext>(new SKACompilerContext(CastChecked<USkillAsset>(BP), InMessageLog, InCompileOptions));
	}
	
	
private :
	void RegisterMenus();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<FUICommandList> PluginCommands;
	
	//for editor
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
	
	TSharedPtr<IAssetTypeActions> SkillAsset2DAction;
	
	void RegisterSettings()
	{
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->RegisterSettings("Project", "Plugins", "SkillEditor2D",
				LOCTEXT("RuntimeSettingsName", "SkillEditor2D"),
				LOCTEXT("RuntimeSettingsDescription", "Configure SkillEditor2D plugin"),
				GetMutableDefault<USkillAsset2DRuntimeSettings>());
		}
	}
	void UnregisterSettings()
	{
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings("Project", "Plugins", "SkillEditor2D");
		}
	}
	
};



#undef LOCTEXT_NAMESPACE