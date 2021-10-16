// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
Pixel2DAnimTransitionSchema.cpp
=============================================================================*/

#include "Pixel2DAnimTransitionSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Pixel2DAnimStateTransitionNode.h"
#include "Pixel2DAnimTransitionGraph.h"
#include "Pixel2DAnimGraphNode_TransitionResult.h"

/////////////////////////////////////////////////////
// UPixel2DAnimTransitionSchema

#define LOCTEXT_NAMESPACE "Pixel2DAnimTransitionSchema"

UPixel2DAnimTransitionSchema::UPixel2DAnimTransitionSchema(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Initialize defaults
}

void UPixel2DAnimTransitionSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Create the entry/exit tunnels
	FGraphNodeCreator<UPixel2DAnimGraphNode_TransitionResult> NodeCreator(Graph);
	UPixel2DAnimGraphNode_TransitionResult* ResultSinkNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(ResultSinkNode, FNodeMetadata::DefaultGraphNode);

	UPixel2DAnimTransitionGraph* TypedGraph = CastChecked<UPixel2DAnimTransitionGraph>(&Graph);
	TypedGraph->MyResultNode = ResultSinkNode;
}

void UPixel2DAnimTransitionSchema::GetGraphDisplayInformation(const UEdGraph& Graph, /*out*/ FGraphDisplayInfo& DisplayInfo) const
{
	DisplayInfo.PlainName = FText::FromString(Graph.GetName());

	const UPixel2DAnimStateTransitionNode* TransNode = Cast<const UPixel2DAnimStateTransitionNode>(Graph.GetOuter());
	DisplayInfo.DisplayName = DisplayInfo.PlainName;
}

void UPixel2DAnimTransitionSchema::HandleGraphBeingDeleted(UEdGraph& GraphBeingRemoved) const
{
	Super::HandleGraphBeingDeleted(GraphBeingRemoved);

	if (UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(&GraphBeingRemoved))
	{
		// Handle composite anim graph nodes
		TArray<UPixel2DAnimStateNodeBase*> StateNodes;
		FBlueprintEditorUtils::GetAllNodesOfClassEx<UPixel2DAnimStateTransitionNode>(Blueprint, StateNodes);

		TSet<UPixel2DAnimStateNodeBase*> NodesToDelete;
		for (int32 i = 0; i < StateNodes.Num(); ++i)
		{
			UPixel2DAnimStateNodeBase* StateNode = StateNodes[i];
			if (StateNode->GetBoundGraph() == &GraphBeingRemoved)
			{
				NodesToDelete.Add(StateNode);
			}
		}

		// Delete the node that owns us
		ensure(NodesToDelete.Num() <= 1);
		for (TSet<UPixel2DAnimStateNodeBase*>::TIterator It(NodesToDelete); It; ++It)
		{
			UPixel2DAnimStateNodeBase* NodeToDelete = *It;

			FBlueprintEditorUtils::RemoveNode(Blueprint, NodeToDelete, true);

			// Prevent re-entrancy here
			NodeToDelete->ClearBoundGraph();
		}
	}
}

#undef LOCTEXT_NAMESPACE
