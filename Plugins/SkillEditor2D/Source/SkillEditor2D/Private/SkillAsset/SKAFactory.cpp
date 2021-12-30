// Fill out your copyright notice in the Description page of Project Settings.


#include "SKAFactory.h"

#include "AssetToolsModule.h"
#include "BlueprintEditorUtils.h"
#include "KismetEditorUtilities.h"

#include "SkillAsset.h"
#include "USKAInstance.h"
#define LOCTEXT_NAMESPACE "USKAFactory"
USKAFactory::USKAFactory(const FObjectInitializer& ObjectInitializer)
{
	SupportedClass=USkillAsset::StaticClass();
	bCreateNew=true;
	bEditAfterNew=true;
	ParentClass=USKAInstance::StaticClass();
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

UObject* USKAFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(InClass->IsChildOf(USkillAsset::StaticClass()));
	USkillAsset * SKABP = CastChecked<USkillAsset>(FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, InName, BlueprintType, USkillAsset::StaticClass(),UBlueprintGeneratedClass::StaticClass(), CallingContext));
	
	return SKABP;
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