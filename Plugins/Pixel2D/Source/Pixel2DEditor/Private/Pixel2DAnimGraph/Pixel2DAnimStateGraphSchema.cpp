// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimStateGraphSchema.h"
#include "Pixel2DAnimStateGraph.h"
#include "Pixel2DAnimStateNode.h"
#include "Pixel2DAnimGraphNode_StateResult.h"
#define LOCTEXT_NAMESPACE "Pixel2DAnimStateGraphSchema"


/////////////////////////////////////////////////////
// UPixel2DAnimStateGraphSchema

UPixel2DAnimStateGraphSchema::UPixel2DAnimStateGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPixel2DAnimStateGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Create the result Create
	FGraphNodeCreator<UPixel2DAnimGraphNode_StateResult> NodeCreator(Graph);
	UPixel2DAnimGraphNode_StateResult* ResultSinkNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(ResultSinkNode, FNodeMetadata::DefaultGraphNode);

	UPixel2DAnimStateGraph* TypedGraph = CastChecked<UPixel2DAnimStateGraph>(&Graph);
	TypedGraph->MyResultNode = ResultSinkNode;
}

void UPixel2DAnimStateGraphSchema::GetGraphDisplayInformation(const UEdGraph& Graph, /*out*/ FGraphDisplayInfo& DisplayInfo) const
{
	DisplayInfo.PlainName = FText::FromString(Graph.GetName());

	if (const UPixel2DAnimStateNode* StateNode = Cast<const UPixel2DAnimStateNode>(Graph.GetOuter()))
	{
		DisplayInfo.PlainName = FText::Format(LOCTEXT("StateNameGraphTitle", "{0} (state)"), FText::FromString(StateNode->GetStateName()));
	}

	DisplayInfo.DisplayName = DisplayInfo.PlainName;
}

#undef LOCTEXT_NAMESPACE
