// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "SkillAsset.h"
#include "SkillAssetFactory.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillAssetAction :public FAssetTypeActions_Base
{
public:
	SkillAssetAction();
	~SkillAssetAction();
	
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;

	virtual UClass* GetSupportedClass() const override;
	
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
		TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual uint32 GetCategories() override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
};
