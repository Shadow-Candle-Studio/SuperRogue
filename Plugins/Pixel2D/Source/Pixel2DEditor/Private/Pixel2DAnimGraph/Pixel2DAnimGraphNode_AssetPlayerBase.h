// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Pixel2DAnimGraphNode_Base.h"
#include "Pixel2DAnimGraphNode_AssetPlayerBase.generated.h"

/** Get the default anim node class for playing a particular asset */
PIXEL2DEDITOR_API UClass* GetNodeClassForAsset(const UClass* AssetClass);

/** See if a particular anim NodeClass can play a particular anim AssetClass */
PIXEL2DEDITOR_API bool SupportNodeClassForAsset(const UClass* AssetClass, UClass* NodeClass);

class UPaperFlipbook;

/** Helper / intermediate for asset player graphical nodes */
UCLASS(Abstract, MinimalAPI)
class UPixel2DAnimGraphNode_AssetPlayerBase : public UPixel2DAnimGraphNode_Base
{
	GENERATED_BODY()
public:
	/** UEdGraphNode interface */
	PIXEL2DEDITOR_API virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	PIXEL2DEDITOR_API virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;

	virtual void SetAnimationAsset(UPaperFlipbook* Asset) { check(false); /*Base function called*/ }
};
