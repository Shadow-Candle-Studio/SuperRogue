// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "Factories/Factory.h"
#include "SkillAssetFactory.generated.h"
static EAssetTypeCategories::Type SkillAssetTypeCategory;
/**
 * 
 */
UCLASS(HideCategories=(Object))
class SKILLEDITORRUNTIME_API USkillAssetFactory : public UFactory
{
	GENERATED_BODY()
public:
	USkillAssetFactory(const FObjectInitializer& ObjectInitializer);
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
	virtual uint32 GetMenuCategories() const override; 
	
};
