// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "AssetTypeActions_Blueprint.h"
#include "SkillAsset.h"


/**
 * 
 */
class SKILLEDITOR2D_API SkillAssetAction :public FAssetTypeActions_Blueprint
{
public:
	SkillAssetAction(EAssetTypeCategories::Type Cat);
	
	
	~SkillAssetAction();
	
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;

	virtual UClass* GetSupportedClass() const override;
	//virtual UThumbnailInfo* GetThumbnailInfo(UObject* Asset) const override;
	
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
		TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
	virtual bool CanFilter() override;
	
	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const override;
	EAssetTypeCategories::Type MyCat;
};
