// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Pixel2DAnimGraph.h"
#include "Pixel2DAnimGraphNode_StateResult.h"
#include "Pixel2DAnimStateGraph.generated.h"

UCLASS(MinimalAPI)
class UPixel2DAnimStateGraph : public UPixel2DAnimGraph
{
	GENERATED_UCLASS_BODY()

	// Result node within the state's animation graph
	UPROPERTY()
	class UPixel2DAnimGraphNode_StateResult* MyResultNode;

	PIXEL2DEDITOR_API class UPixel2DAnimGraphNode_StateResult* GetResultNode();
};

