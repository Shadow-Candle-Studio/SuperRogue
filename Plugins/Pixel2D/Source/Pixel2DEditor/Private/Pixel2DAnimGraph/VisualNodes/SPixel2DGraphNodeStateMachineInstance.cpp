// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SPixel2DGraphNodeStateMachineInstance.h"
#include "Pixel2DAnimStateMachineGraph.h"
#include "Pixel2DAnimGraphNode_StateMachineBase.h"

/////////////////////////////////////////////////////
// SPixel2DGraphNodeStateMachineInstance

void SPixel2DGraphNodeStateMachineInstance::Construct(const FArguments& InArgs, UPixel2DAnimGraphNode_StateMachineBase* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

UEdGraph* SPixel2DGraphNodeStateMachineInstance::GetInnerGraph() const
{
	UPixel2DAnimGraphNode_StateMachineBase* StateMachineInstance = CastChecked<UPixel2DAnimGraphNode_StateMachineBase>(GraphNode);

	return StateMachineInstance->EditorStateMachineGraph;
}
