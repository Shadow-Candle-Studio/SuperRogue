// Fill out your copyright notice in the Description page of Project Settings.


#include "SKABFactory.h"

#include "AssetTypeCategories.h"
#include "FileHelper.h"
#include "SKAFactory.h"
#include "SkillAsset.h"


USKABFactory::USKABFactory(
	const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	Formats.Add(FString(TEXT("SKA;"))+
		NSLOCTEXT("USkillAssetFactoryFromBinary","FormatTxt", "SkillAsset file").ToString());
	SupportedClass=USkillAsset::StaticClass();
	bCreateNew=false;
	bEditorImport=true;
}

UObject* USKABFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, UObject* Context, const TCHAR* String, const uint8*& Buffer, const uint8* BufferEnd,
	FFeedbackContext* Warn)
{
	USkillAsset* SkillAsset=nullptr;
	FString TextString;
	if(FFileHelper::LoadFileToString(TextString,*CurrentFilename))
	{
		SkillAsset=NewObject<USkillAsset>(InParent, InClass, InName, Flags);
		SkillAsset->text=FText::FromString(TextString);
	}
	
	return SkillAsset;
	
}

UObject* USKABFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
														 EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
														 bool& bOutOperationCanceled)
{
	USkillAsset* SkillAsset=nullptr;
	FString TextString;
	if(FFileHelper::LoadFileToString(TextString,*Filename))
	{
		SkillAsset=NewObject<USkillAsset>
		(InParent, InClass,
			InName, Flags);
		SkillAsset->text=FText::FromString(TextString);
	}
	bOutOperationCanceled=false;
	return SkillAsset;
}

uint32 USKABFactory::GetMenuCategories() const
{
	return SKACategory;
}
