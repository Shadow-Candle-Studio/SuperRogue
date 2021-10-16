// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimStateGraph.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnimStateGraph"

/////////////////////////////////////////////////////
// UPixel2DAnimStateGraph

UPixel2DAnimStateGraph::UPixel2DAnimStateGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPixel2DAnimGraphNode_StateResult* UPixel2DAnimStateGraph::GetResultNode()
{
	return MyResultNode;
}

#undef LOCTEXT_NAMESPACE
