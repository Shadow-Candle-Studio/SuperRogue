// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Pixel2DAnimGraph.h"
#include "Pixel2DAnimTransitionGraph.generated.h"

UCLASS(MinimalAPI)
class UPixel2DAnimTransitionGraph : public UPixel2DAnimGraph
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	class UPixel2DAnimGraphNode_TransitionResult* MyResultNode;

	PIXEL2DEDITOR_API class UPixel2DAnimGraphNode_TransitionResult* GetResultNode();
};

