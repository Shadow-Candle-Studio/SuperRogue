// Fill out your copyright notice in the Description page of Project Settings.


#include "SKAFactory.h"

#include "AssetToolsModule.h"
#include "AssetTypeCategories.h"
#include "SkillAsset.h"
#define LOCTEXT_NAMESPACE "USKAFactory"
USKAFactory::USKAFactory(const FObjectInitializer& ObjectInitializer)
{
	SupportedClass=USkillAsset::StaticClass();
	bCreateNew=true;
	bEditAfterNew=true;
	auto AssetToolsModule=&FModuleManager::
	LoadModuleChecked<FAssetToolsModule>("AssetTools").
	Get();
	SKACategory=AssetToolsModule->
	RegisterAdvancedAssetCategory(FName
		(
		TEXT("SkillAsset")),
		
LOCTEXT("SkillEditor2D","Skill Editor Assets")
	);
}

UObject* USKAFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                       UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<USkillAsset>(InParent,InClass,InName,Flags);
}

bool USKAFactory::ShouldShowInNewMenu() const
{
	return true;
}

uint32 USKAFactory::GetMenuCategories() const
{
	return SKACategory;
}
#undef LOCTEXT_NAMESPACE