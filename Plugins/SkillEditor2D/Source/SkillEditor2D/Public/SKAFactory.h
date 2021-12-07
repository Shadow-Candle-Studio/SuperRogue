// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "Object.h"
#include "SKAFactory.generated.h"
static EAssetTypeCategories::Type SKACategory;
/**
 * 
 */
UCLASS(HideCategories=(Object))
class SKILLEDITOR2D_API USKAFactory : public UFactory
{
	GENERATED_BODY()
public:
	USKAFactory(const FObjectInitializer& ObjectInitializer);
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
	virtual uint32 GetMenuCategories() const override; 
};
