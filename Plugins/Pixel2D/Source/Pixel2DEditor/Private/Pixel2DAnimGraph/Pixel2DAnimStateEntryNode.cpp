// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
Pixel2DAnimStateNode.cpp
=============================================================================*/

#include "Pixel2DAnimStateEntryNode.h"
#include "EdGraph/EdGraph.h"
#include "Pixel2DAnimStateMachineSchema.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnimStateEntryNode"

/////////////////////////////////////////////////////
// UPixel2DAnimStateEntryNode

UPixel2DAnimStateEntryNode::UPixel2DAnimStateEntryNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPixel2DAnimStateEntryNode::AllocateDefaultPins()
{
	const UPixel2DAnimStateMachineSchema* Schema = GetDefault<UPixel2DAnimStateMachineSchema>();
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, Schema->PC_Exec, TEXT("Entry"));
}

FText UPixel2DAnimStateEntryNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UEdGraph* Graph = GetGraph();
	return FText::FromString(Graph->GetName());
}

FText UPixel2DAnimStateEntryNode::GetTooltipText() const
{
	return LOCTEXT("StateEntryNodeTooltip", "Entry point for state machine");
}

UEdGraphNode* UPixel2DAnimStateEntryNode::GetOutputNode() const
{
	if (Pins.Num() > 0 && Pins[0] != NULL)
	{
		check(Pins[0]->LinkedTo.Num() <= 1);
		if (Pins[0]->LinkedTo.Num() > 0 && Pins[0]->LinkedTo[0]->GetOwningNode() != NULL)
		{
			return Pins[0]->LinkedTo[0]->GetOwningNode();
		}
	}
	return NULL;
}

#undef LOCTEXT_NAMESPACE
