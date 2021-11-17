// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillEditorRuntime.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "IPluginManager.h"
#include "SkillAssetAction.h"
#include "SkillAssetFactory.h"
#include "SlateStyleRegistry.h"


#define LOCTEXT_NAMESPACE "FSkillEditorRuntimeModule"

void FSkillEditorRuntimeModule::StartupModule()
{
	//asset registration!s
	IAssetTools& AssetToolsModule=FModuleManager::
	LoadModuleChecked<FAssetToolsModule>("AssetTools").
	Get();
	
	SkillAssetTypeCategory=AssetToolsModule.
	RegisterAdvancedAssetCategory(FName
		(
		TEXT("SkillAsset")),
		
LOCTEXT("SkillEditor2D","Skill Editor Assets")
	);
	
	AssetToolsModule.RegisterAssetTypeActions(MakeShareable
		(new SkillAssetAction));


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
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	

	
}

void FSkillEditorRuntimeModule::ShutdownModule()
{
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
}

bool FSkillEditorRuntimeModule::IsGameModule() const
{
	return false;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSkillEditorRuntimeModule, SkillEditorRuntime)