// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimAssetTypeActions.h"
#include "Pixel2DAnimBlueprintEditor.h"
#include "Pixel2DAnimBlueprint.h"

#include "Pixel2DAnimBlueprintFactory.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FPixel2DAnimAssetTypeActions

FPixel2DAnimAssetTypeActions::FPixel2DAnimAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FPixel2DAnimAssetTypeActions::GetName() const
{
	return LOCTEXT("FSpriteAnimAssetTypeActionsName", "SpriteAnim");
}

FColor FPixel2DAnimAssetTypeActions::GetTypeColor() const
{
	return FColor::Emerald;
}

UClass* FPixel2DAnimAssetTypeActions::GetSupportedClass() const
{
	return UPixel2DAnimBlueprint::StaticClass();
}

void FPixel2DAnimAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UPixel2DAnimBlueprint* SpriteBP = Cast<UPixel2DAnimBlueprint>(*ObjIt))
		{
			const bool bBringToFrontIfOpen = true;
			if (IAssetEditorInstance* EditorInstance = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(SpriteBP, bBringToFrontIfOpen))
			{
				EditorInstance->FocusWindow(SpriteBP);
			}
			else
			{
				TSharedRef<FPixel2DAnimBlueprintEditor > NewSpriteAnimBlueprintEditor(new FPixel2DAnimBlueprintEditor());
				NewSpriteAnimBlueprintEditor->InitSpriteAnimBlueprintEditor(Mode, EditWithinLevelEditor, SpriteBP);
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadCorruptAnimBlueprint", "The Anim Blueprint could not be loaded because it is corrupt."));
		}
	}
}

uint32 FPixel2DAnimAssetTypeActions::GetCategories()
{
	return MyAssetCategory;
}

UFactory* FPixel2DAnimAssetTypeActions::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UPixel2DAnimBlueprintFactory* AnimBlueprintFactory = NewObject<UPixel2DAnimBlueprintFactory>();
	return AnimBlueprintFactory;
}

#undef LOCTEXT_NAMESPACE
