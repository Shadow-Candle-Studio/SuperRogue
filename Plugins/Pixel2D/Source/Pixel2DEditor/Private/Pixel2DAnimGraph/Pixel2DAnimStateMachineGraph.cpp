// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimStateMachineGraph.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnimGraph"

/////////////////////////////////////////////////////
// UPixel2DAnimStateMachineGraph

UPixel2DAnimStateMachineGraph::UPixel2DAnimStateMachineGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAllowDeletion = false;
	bAllowRenaming = true;
}


#undef LOCTEXT_NAMESPACE
