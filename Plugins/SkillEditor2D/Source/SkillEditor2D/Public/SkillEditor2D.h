// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowTabManager.h"


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





class SKILLEDITOR2D_API ISkillAssetEditorModule_Base:
public IModuleInterface,
public IHasMenuExtensibility,
public IHasToolBarExtensibility
{
public:
	ISkillAssetEditorModule_Base();
	~ISkillAssetEditorModule_Base();
	virtual TSharedRef<ISkillAssetEditor> CreateCustomAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, USkillAsset* CustomAsset) = 0;
};






class FSkillEditor2DModule : public ISkillAssetEditorModule_Base
{
public:
	// TSharedPtr<FSkillEditorViewPortRenderingClient> RealRenderingClient;
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void PluginButtonClicked();

	virtual TSharedRef<ISkillAssetEditor> CreateCustomAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, USkillAsset* CustomAsset) override;
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override
	{
		return  MenuExtensibilityManager;
	}
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override
	{
		return ToolBarExtensibilityManager;
	}
	
private :
	void RegisterMenus();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<FUICommandList> PluginCommands;

	//for editor
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
	TSharedPtr<FSlateStyleSet> StyleSet;
	TSharedPtr<IAssetTypeActions> SkillAsset2DAction;
	IAssetTools* AssetToolsModule;
	

};
