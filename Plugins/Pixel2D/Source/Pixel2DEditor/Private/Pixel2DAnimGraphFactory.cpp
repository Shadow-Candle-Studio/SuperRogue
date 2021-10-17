// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimGraphFactory.h"

#include "Pixel2DAnimGraph/Pixel2DAnimGraphNode_Root.h"
#include "Pixel2DAnimGraph/Pixel2DAnimGraphNode_StateMachineBase.h"

#include "Pixel2DAnimStateTransitionNode.h"
#include "Pixel2DAnimStateNode.h"
#include "Pixel2DAnimStateMachineSchema.h"
#include "Pixel2DAnimGraphSchema.h"

#include "SPixel2DGraphNodeTransition.h"
#include "SPixel2DGraphNodeConduit.h"
#include "SPixel2DGraphNodeStateEntry.h"

#include "SPixel2DGraphNodeResult.h"
#include "SPixel2DGraphNodeNode.h"
#include "SPixel2DGraphNodeStateMachineInstance.h"

#include "Pixel2DAnimGraph/Pixel2DStateMachineConnectionDrawingPolicy.h"
#include "Pixel2DAnimGraph/Pixel2DAnimGraphConnectionDrawingPolicy.h"

#include "KismetPins/SGraphPinExec.h"


TSharedPtr<class SGraphNode> FPixel2DAnimGraphNodeFactory::CreateNode(class UEdGraphNode* InNode) const
{
	if (UPixel2DAnimGraphNode_Base* BaseAnimNode = Cast<UPixel2DAnimGraphNode_Base>(InNode))
	{
		if (UPixel2DAnimGraphNode_Root* RootAnimNode = Cast<UPixel2DAnimGraphNode_Root>(InNode))
		{
			return SNew(SPixel2DGraphNodeResult, RootAnimNode);
		}
		else if (UPixel2DAnimGraphNode_StateMachineBase* StateMachineInstance = Cast<UPixel2DAnimGraphNode_StateMachineBase>(InNode))
		{
			return SNew(SPixel2DGraphNodeStateMachineInstance, StateMachineInstance);
		}
		else
		{
			return SNew(SPixel2DGraphNodeNode, BaseAnimNode);
		}
	}
	else if (UPixel2DAnimStateTransitionNode* TransitionNode = Cast<UPixel2DAnimStateTransitionNode>(InNode))
	{
		return SNew(SPixel2DGraphNodeTransition, TransitionNode);
	}
	else if (UPixel2DAnimStateNode* StateNode = Cast<UPixel2DAnimStateNode>(InNode))
	{
		return SNew(SPixel2DGraphNodeState, StateNode);
	}
	else if (UPixel2DAnimStateEntryNode* EntryNode = Cast<UPixel2DAnimStateEntryNode>(InNode))
	{
		return SNew(SPixel2DGraphNodeStateEntry, EntryNode);
	}

	return nullptr;
}

TSharedPtr<class SGraphPin> FPixel2DAnimGraphPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		if (InPin->PinType.PinSubCategoryObject == FPixel2DPoseLink::StaticStruct())
		{
			return SNew(SGraphPinExec, InPin);
		}
	}

	if (InPin->PinType.PinCategory == UPixel2DAnimStateMachineSchema::PC_Exec)
	{
		return SNew(SGraphPinExec, InPin);
	}

	return nullptr;
}


class FConnectionDrawingPolicy* FPixel2DAnimGraphPinConnectionFactory::CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	if (Schema->IsA(UPixel2DAnimGraphSchema::StaticClass()))
	{
		return new FPixel2DAnimGraphConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}
	else if (Schema->IsA(UPixel2DAnimStateMachineSchema::StaticClass()))
	{
		return new FPixel2DStateMachineConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}
	return nullptr;
}