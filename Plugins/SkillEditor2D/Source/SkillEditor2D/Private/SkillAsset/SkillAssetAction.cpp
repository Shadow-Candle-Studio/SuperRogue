// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetAction.h"

#include "SKAFactory.h"
#include "SkillAssetEditor.h"
#include "SkillEditor2D.h"
#define LOCTEXT_NAMESPACE "SkillAssetActions"
SkillAssetAction::SkillAssetAction(EAssetTypeCategories::Type Cat): MyCat(Cat)
{
	
}



SkillAssetAction::~SkillAssetAction()
{
	
}

FText SkillAssetAction::GetName() const
{
	return NSLOCTEXT("AssetTypeActions",
			"SkillAsset Action","Skill Asset");
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
		if (USkillAsset* SkA = Cast<USkillAsset>(*ObjIt))
		{
			const bool bBringToFrontIfOpen = true;
			if (IAssetEditorInstance* EditorInstance = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(SkA, bBringToFrontIfOpen))
			{
				EditorInstance->FocusWindow(SkA);
			}
			else
			{
				TSharedRef<FSkillAssetEditor > NewSkillAssetEditorEditor(new FSkillAssetEditor());
				NewSkillAssetEditorEditor->InitSkillAssetEditor(Mode, EditWithinLevelEditor, SkA);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadCorruptAnimBlueprint", "The Anim Blueprint could not be loaded because it is corrupt."));
		}
	}
}

uint32 SkillAssetAction::GetCategories()
{
	return SKACategory;
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
							//&& !SkillAsset->text.IsEmpty()
							if (IsValid(SkillAsset) )
							{
								//SkillAsset->text = FText::FromString("");
								SkillAsset->PostEditChange();
								SkillAsset->MarkPackageDirty();
							}
						}
					}),
					FCanExecuteAction::CreateLambda([=] {
						for (auto& SkillAsset : SkillAssets)
						{
							//&& !SkillAsset->text.IsEmpty()
							if (IsValid(SkillAsset) )
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
