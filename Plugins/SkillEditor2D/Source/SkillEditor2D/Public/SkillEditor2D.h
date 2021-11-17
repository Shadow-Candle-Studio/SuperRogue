// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

class FToolBarBuilder;
class FMenuBuilder;
class USkillAsset;



class SKILLEDITOR2D_API ISkillAssetEditor : public FAssetEditorToolkit
{
public:
	virtual USkillAsset* GetSkillAsset()=0;
	virtual void SetSkillAsset(USkillAsset* InSkillAsset) =0;
};

class SKILLEDITOR2D_API FSkillAssetEditor: public ISkillAssetEditor
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	void InitSkillAssetEditor
	(const EToolkitMode::Type Mode,
	const TSharedPtr<class IToolkitHost>& InitToolkitHost,
	USkillAsset* InSkillAsset);
	virtual ~FSkillAssetEditor() override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override;

	virtual USkillAsset* GetSkillAsset() override;
	virtual void SetSkillAsset(USkillAsset* InSkillAsset) override;
    
	
private:
	TSharedRef<SDockTab> SpawnPropertiesTab(const FSpawnTabArgs& Args);
	
	TSharedPtr< SDockableTab > PropertiesTab;


	TSharedPtr<class IDetailsView> DetailsView;
	static const FName	ToolkitFName;
	static const FName PropertiesTabId;
	USkillAsset* SkillAsset = nullptr;
};

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

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void PluginButtonClicked();
	virtual TSharedRef<ISkillAssetEditor> CreateCustomAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, USkillAsset* CustomAsset);
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
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
};
