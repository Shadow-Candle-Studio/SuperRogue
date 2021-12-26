// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAsset.h"


#include "USKAGeneratedClassBP.h"

USkillAsset::USkillAsset()
{
	UE_LOG(LogTemp,Warning,L"USKILLAsset created")
}

void USkillAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

EDataValidationResult USkillAsset::IsDataValid(TArray<FText>& ValidationErrors)
{
	return GeneratedClass ? GeneratedClass->GetDefaultObject()->IsDataValid(ValidationErrors) : EDataValidationResult::Invalid;
}

UClass* USkillAsset::GetBlueprintClass() const
{
	return USKAGeneratedClassBP::StaticClass();
}

bool USkillAsset::CanRecompileWhilePlayingInEditor() const
{
	return true;
}

void USkillAsset::PostLoad()
{
	Super::PostLoad();
}
