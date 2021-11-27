// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillEditorRuntime.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "IPluginManager.h"

#include "SkillAssetFactory.h"
#include "SlateStyleRegistry.h"
#include "SkillAssetAction.h"

#define LOCTEXT_NAMESPACE "FSkillEditorRuntimeModule"

void FSkillEditorRuntimeModule::StartupModule()
{
	//asset registration!s
	AssetToolsModule=&FModuleManager::
	LoadModuleChecked<FAssetToolsModule>("AssetTools").
	Get();
	
	SkillAssetTypeCategory=AssetToolsModule->
	RegisterAdvancedAssetCategory(FName
		(
		TEXT("SkillAsset")),
		
LOCTEXT("SkillEditor2D","Skill Editor Assets")
	);
	SkillAsset2DAction=MakeShareable(new SkillAssetAction);
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

	

	
}

void FSkillEditorRuntimeModule::ShutdownModule()
{


	
	//IAssetTools& AssetTools=FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	// if(AssetToolsModule!=nullptr)
	// AssetToolsModule->UnregisterAssetTypeActions(SkillAsset2DAction.ToSharedRef());
	
	

	
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	ensure(StyleSet.IsUnique());
	StyleSet.Reset();
	
}

bool FSkillEditorRuntimeModule::IsGameModule() const
{
	return false;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSkillEditorRuntimeModule, SkillEditorRuntime)