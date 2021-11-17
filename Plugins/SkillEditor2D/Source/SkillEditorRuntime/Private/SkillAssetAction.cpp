// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetAction.h"
#include "SkillEditor2D.h"
#define LOCTEXT_NAMESPACE "SkillAssetActions"
SkillAssetAction::SkillAssetAction()
{
}



SkillAssetAction::~SkillAssetAction()
{
}

FText SkillAssetAction::GetName() const
{
	return NSLOCTEXT("AssetTypeActions",
			"SkillAssetAction","SkillAsset");
}

FColor SkillAssetAction::GetTypeColor() const
{
	return FColor::Green;
}

UClass* SkillAssetAction::GetSupportedClass() const
{
	return USkillAsset::StaticClass();
}

void SkillAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto MySkillAsset = Cast<USkillAsset>(*ObjIt);
		if (MySkillAsset != NULL)
		{
			ISkillAssetEditorModule_Base* CustomAssetEditorModule = &FModuleManager::LoadModuleChecked<ISkillAssetEditorModule_Base>("SkillEditor2D");
			CustomAssetEditorModule->CreateCustomAssetEditor(Mode, EditWithinLevelEditor, MySkillAsset);
		}
	}
}

uint32 SkillAssetAction::GetCategories()
{
	return SkillAssetTypeCategory;
}

bool SkillAssetAction::HasActions(const TArray<UObject*>& InObjects) const
{
	return true;
}

void SkillAssetAction::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
	auto SkillAssets=GetTypedObjectPtrs<USkillAsset>(InObjects);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("SkillAssetClearAsset","Clear Asset"),
		LOCTEXT("SkillAssetClearAssetToolTip","Clear the asset contens in a text"),
        FSlateIcon(),
        FUIAction(
					FExecuteAction::CreateLambda([=]{
						for (auto& SkillAsset : SkillAssets)
						{
							if (IsValid(SkillAsset) && !SkillAsset->text.IsEmpty())
							{
								SkillAsset->text = FText::FromString("");
								SkillAsset->PostEditChange();
								SkillAsset->MarkPackageDirty();
							}
						}
					}),
					FCanExecuteAction::CreateLambda([=] {
						for (auto& SkillAsset : SkillAssets)
						{
							if (IsValid(SkillAsset) && !SkillAsset->text.IsEmpty())
							{
								return true;
							}
						}
						return false;
					})
				)
				);
	
}

FText SkillAssetAction::GetAssetDescription(const FAssetData& AssetData) const
{
	
	return FText::FromString("Skill Asset stores all scripts of a special skill");
	
}

bool SkillAssetAction::CanFilter()
{
	return true;
}
#undef LOCTEXT_NAMESPACE
