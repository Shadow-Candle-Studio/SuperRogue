// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditorModes.h"
#include "Pixel2DAnimBlueprintEditor.h"
#include "Pixel2DAnimBlueprint.h"

class FPixel2DAnimBlueprintEditorMode : public FBlueprintEditorApplicationMode
{
protected:
	// Set of spawnable tabs in persona mode (@TODO: Multiple lists!)
	FWorkflowAllowedTabSet TabFactories;

public:
	FPixel2DAnimBlueprintEditorMode(const TSharedRef<FPixel2DAnimBlueprintEditor>& InAnimationBlueprintEditor);

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	// End of FApplicationMode interface

private:
	TWeakObjectPtr<class UPixel2DAnimBlueprint> AnimBlueprintPtr;
};