// Fill out your copyright notice in the Description page of Project Settings.


#include "BPGraphFactories.h"

#include "AnimStateTransitionNode.h"
#include "GraphNodeState.h"
#include "SKADrawingConnectionPolicy.h"
#include "SKAEntryNode.h"
#include "SKANode.h"
#include "SKAOutputPin.h"
#include "SKAUEdGraphSchema.h"
#include "TransitionSGraphNode.h"


TSharedPtr<SGraphNode> SKAGraphNodeFactory::CreateNode(UEdGraphNode* InNode) const
{
	if (USKANode* StateNode = Cast<USKANode>(InNode))
	{
		return SNew(GraphNodeState, StateNode);
	}
	else if (USKAEntryNode* EntryNode = Cast<USKAEntryNode>(InNode))
	{
		return SNew(SGraphSKAEntryNode,EntryNode);
	}
	else if (UAnimStateTransitionNode* TransitionNode = Cast<UAnimStateTransitionNode>(InNode))
	{
		return SNew(TransitionSGraphNode, TransitionNode);
	}
	return nullptr;
	
}

TSharedPtr<SGraphPin> SKAGraphPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec)
	{
		return SNew(SKAOutputPin, Pin);
	}
	return nullptr;
}

FConnectionDrawingPolicy* SKAGraphPinConnectionFactory::CreateConnectionPolicy(const UEdGraphSchema* Schema,
	int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect,
	FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	if (Schema->IsA(USKAUEdGraphSchema::StaticClass()))
	{
		return new SKADrawingConnectionPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}
	return nullptr;
}
