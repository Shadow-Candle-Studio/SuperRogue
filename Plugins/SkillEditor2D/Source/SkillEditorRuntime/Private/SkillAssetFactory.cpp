// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetFactory.h"

#include "AssetTypeCategories.h"
#include "SkillAsset.h"



USkillAssetFactory::USkillAssetFactory(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	SupportedClass=USkillAsset::StaticClass();
	bCreateNew=true;
	bEditAfterNew=true;
}

UObject* USkillAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                              UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<USkillAsset>(InParent,InClass,InName,Flags);
}

bool USkillAssetFactory::ShouldShowInNewMenu() const
{
	return true;
}

uint32 USkillAssetFactory::GetMenuCategories() const
{
	return SkillAssetTypeCategory;
}


