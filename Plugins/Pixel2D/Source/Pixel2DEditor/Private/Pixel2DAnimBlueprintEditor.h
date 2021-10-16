// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditor.h"

#include "Pixel2DAnimNotifyEditor.h"

struct FPixel2DAnimBlueprintEditorModes
{
	// Mode constants
	static const FName AnimationBlueprintEditorMode;
	static FText GetLocalizedMode(const FName InMode)
	{
		static TMap< FName, FText > LocModes;

		if (LocModes.Num() == 0)
		{
			LocModes.Add(AnimationBlueprintEditorMode, NSLOCTEXT("SpriteAnimationBlueprintEditorModes", "SpriteAnimationBlueprintEditorMode", "Sprite Animation Blueprint"));
		}

		check(InMode != NAME_None);
		const FText* OutDesc = LocModes.Find(InMode);
		check(OutDesc);
		return *OutDesc;
	}
private:
	FPixel2DAnimBlueprintEditorModes() {}
};


namespace Pixel2DAnimationBlueprintEditorTabs
{
	extern const FName AssetBrowserTab;
};


class FPixel2DAnimBlueprintEditor : public FBlueprintEditor
{

public:

	void InitSpriteAnimBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class UPixel2DAnimBlueprint* InSpriteAnimBlueprint);

public:

	// Gets the Anim Blueprint being edited/viewed by this Persona instance
	UPixel2DAnimBlueprint* GetSpriteAnimBlueprint() const;

	/** Get the object to be displayed in the asset properties */
	UObject* HandleGetObject();

public:

	/** Returns a pointer to the Blueprint object we are currently editing, as long as we are editing exactly one */
	virtual UBlueprint* GetBlueprintObj() const override;

	/** Handle opening a new asset from the asset browser */
	void HandleOpenNewAsset(UObject * InNewAsset);

protected:
	//~ Begin FBlueprintEditor Interface
	virtual void OnOpenRelatedAsset() override;
	//~ End FBlueprintEditor Interface

	/** Extend menu */
	void ExtendMenu();

	/** Extend toolbar */
	void ExtendToolbar();

	/** The extender to pass to the level editor to extend it's window menu */
	TSharedPtr<FExtender> MenuExtender;

	/** Toolbar extender */
	TSharedPtr<FExtender> ToolbarExtender;

	/** AnimNotifyEditor **/
	TSharedPtr<FPixel2DAnimNotifyEditor> AnimNotifyEditor;

	/** Handle to the AnimNotifyEditor Tab Spawner**/
	FTabSpawnerEntry * AnimNotifyEditorTabSpawner = NULL;
};