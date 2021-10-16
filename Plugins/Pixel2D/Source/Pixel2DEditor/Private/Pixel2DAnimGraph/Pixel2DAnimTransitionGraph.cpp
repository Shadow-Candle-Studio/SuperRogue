// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimTransitionGraph.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnimStateGraph"

/////////////////////////////////////////////////////
// UPixel2DAnimTransitionGraph

UPixel2DAnimTransitionGraph::UPixel2DAnimTransitionGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UPixel2DAnimGraphNode_TransitionResult* UPixel2DAnimTransitionGraph::GetResultNode()
{
	return MyResultNode;
}

#undef LOCTEXT_NAMESPACE
