// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Modules/ModuleInterface.h"

#define PIXEL2D_EDITOR_MODULE_NAME "Pixel2DEditor"

//////////////////////////////////////////////////////////////////////////
// IPixel2DAnimEditorModule

class IPixel2DAnimEditorModule : public IModuleInterface
{

private:
	/** When a new AnimBlueprint is created, this will handle post creation work such as adding non-event default nodes */
	void OnNewBlueprintCreated(class UBlueprint* InBlueprint);

public:
	virtual TSharedRef<class FWorkflowTabFactory> CreateAnimationAssetBrowserTabFactory(const TSharedRef<class FWorkflowCentricApplication>& InHostingApp, bool bInShowHistory) const = 0;
};