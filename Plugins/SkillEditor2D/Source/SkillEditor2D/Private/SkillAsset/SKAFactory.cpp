// Fill out your copyright notice in the Description page of Project Settings.


#include "SKAFactory.h"

#include "AssetToolsModule.h"
#include "BlueprintEditorUtils.h"
#include "KismetEditorUtilities.h"
#include "MovieSceneToolsProjectSettings.h"

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
	
	USkillAsset* Asset=NewObject<USkillAsset>(InParent,InClass,InName,Flags);
	FillInSequenceData(Asset);
	return Asset;
	
}

UObject* USKAFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(InClass->IsChildOf(USkillAsset::StaticClass()));
	USkillAsset * SKABP = CastChecked<USkillAsset>(FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, InName, BlueprintType, USkillAsset::StaticClass(),UBlueprintGeneratedClass::StaticClass(), CallingContext));
	FillInSequenceData(SKABP);
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

void USKAFactory::FillInSequenceData(USkillAsset* Inskill)
{
	ULevelSequence* NewLevelSequence = NewObject<ULevelSequence>(Inskill,"SkillAssetSequence", RF_Transactional);
	NewLevelSequence->Initialize();
	// Set up some sensible defaults
	const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();
	FFrameRate TickResolution = NewLevelSequence->GetMovieScene()->GetTickResolution();
	NewLevelSequence->GetMovieScene()->SetPlaybackRange((ProjectSettings->DefaultStartTime*TickResolution).FloorToFrame(), (ProjectSettings->DefaultDuration*TickResolution).FloorToFrame().Value);
	FMemory::Memcpy((void*)Inskill->SequenceData,(void*)NewLevelSequence,sizeof(*NewLevelSequence));
	
	UE_LOG(LogTemp,Warning,L"Skillaseet Sequence size is %s",*Inskill->GetSequenceData()->GetName())
}
#undef LOCTEXT_NAMESPACE
