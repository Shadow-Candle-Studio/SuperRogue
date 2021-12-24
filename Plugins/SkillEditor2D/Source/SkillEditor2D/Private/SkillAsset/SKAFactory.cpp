// Fill out your copyright notice in the Description page of Project Settings.


#include "SKAFactory.h"

#include "AssetToolsModule.h"
#include "AssetTypeCategories.h"
#include "BlueprintEditorUtils.h"
#include "KismetEditorUtilities.h"
#include "SKAUEdGraph.h"
#include "SKAUEdGraphSchema.h"
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
	USkillAsset * SKABP = CastChecked<USkillAsset>(FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, InName, BlueprintType, USkillAsset::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext));

	// Add the pixel anim graph
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(SKABP, K2Schema->GN_AnimGraph, USKAUEdGraph::StaticClass(), USKAUEdGraphSchema::StaticClass());
	FBlueprintEditorUtils::AddDomainSpecificGraph(SKABP, NewGraph);
	SKABP->LastEditedDocuments.Add(NewGraph);
	NewGraph->bAllowDeletion = false;

	// Mark the BP as being regenerated, so it will not be confused as needing to be loaded and regenerated when a referenced BP loads.
	SKABP->bHasBeenRegenerated = true;

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