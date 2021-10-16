// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimBlueprintUtils.h"

#include "K2Node_Composite.h"
#include "Pixel2DAnimStateNodeBase.h"
#include "Pixel2DAnimStateNode.h"
#include "Pixel2DAnimStateTransitionNode.h"
#include "Pixel2DAnimGraphNode_StateMachineBase.h"
#include "Pixel2DAnimGraphNode_StateMachine.h"
#include "K2Node_Timeline.h"
#include "K2Node_MacroInstance.h"

// Removes the supplied graph from the Blueprint.
void FPixel2DAnimBlueprintUtils::RemoveGraph(UBlueprint* Blueprint, class UEdGraph* GraphToRemove, EGraphRemoveFlags::Type Flags /*= Transient | Recompile */)
{
	struct Local
	{
		static bool IsASubGraph(UEdGraph* Graph)
		{
			UObject* Outer = Graph->GetOuter();
			return (Outer && Outer->IsA(UK2Node_Composite::StaticClass()));
		}
	};

	GraphToRemove->Modify();

	for (UObject* TestOuter = GraphToRemove->GetOuter(); TestOuter; TestOuter = TestOuter->GetOuter())
	{
		if (TestOuter == Blueprint)
		{
			Blueprint->DelegateSignatureGraphs.Remove(GraphToRemove);
			Blueprint->FunctionGraphs.Remove(GraphToRemove);
			Blueprint->UbergraphPages.Remove(GraphToRemove);

			// Can't just call Remove, the object is wrapped in a struct
			for (int EditedDocIdx = 0; EditedDocIdx < Blueprint->LastEditedDocuments.Num(); ++EditedDocIdx)
			{
				if (Blueprint->LastEditedDocuments[EditedDocIdx].EditedObjectPath.ResolveObject() == GraphToRemove)
				{
					Blueprint->LastEditedDocuments.RemoveAt(EditedDocIdx);
					break;
				}
			}

			if (Blueprint->MacroGraphs.Remove(GraphToRemove) > 0)
			{
				//removes all macro nodes using this macro graph
				TArray<UK2Node_MacroInstance*> MacroNodes;
				FBlueprintEditorUtils::GetAllNodesOfClass(Blueprint, MacroNodes);
				for (UK2Node_MacroInstance* Node : MacroNodes)
				{
					if (Node->GetMacroGraph() == GraphToRemove)
					{
						FBlueprintEditorUtils::RemoveNode(Blueprint, Node);
					}
				}

				// Clear the cache since it's indexed by graph and one of the graphs is going away
				FBlueprintEditorUtils::ClearMacroCosmeticInfoCache(Blueprint);
			}

			for (FBPInterfaceDescription& CurrInterface : Blueprint->ImplementedInterfaces)
			{
				CurrInterface.Graphs.Remove(GraphToRemove);
			}
		}
		else if (UEdGraph* OuterGraph = Cast<UEdGraph>(TestOuter))
		{
			// remove ourselves
			OuterGraph->Modify();
			OuterGraph->SubGraphs.Remove(GraphToRemove);
		}
		else if (!(Cast<UK2Node_Composite>(TestOuter) ||
			Cast<UPixel2DAnimStateNodeBase>(TestOuter) ||
			Cast<UPixel2DAnimStateTransitionNode>(TestOuter) ||
			Cast<UPixel2DAnimGraphNode_StateMachineBase>(TestOuter)))
		{
			break;
		}
	}

	// Remove timelines held in the graph
	TArray<UK2Node_Timeline*> AllTimelineNodes;
	GraphToRemove->GetNodesOfClass<UK2Node_Timeline>(AllTimelineNodes);
	for (UK2Node_Timeline* TimelineNode : AllTimelineNodes)
	{
		TimelineNode->DestroyNode();
	}

	// Handle subgraphs held in graph
	TArray<UK2Node_Composite*> AllCompositeNodes;
	GraphToRemove->GetNodesOfClass<UK2Node_Composite>(AllCompositeNodes);

	const bool bDontRecompile = true;
	for (UK2Node_Composite* CompNode : AllCompositeNodes)
	{
		if (CompNode->BoundGraph && Local::IsASubGraph(CompNode->BoundGraph))
		{
			FPixel2DAnimBlueprintUtils::RemoveGraph(Blueprint, CompNode->BoundGraph, EGraphRemoveFlags::None);
		}
	}

	// Animation nodes can contain subgraphs but are not composite nodes, handle their graphs
	TArray<UPixel2DAnimStateNodeBase*> AllAnimCompositeNodes;
	GraphToRemove->GetNodesOfClassEx<UPixel2DAnimStateNode>(AllAnimCompositeNodes);
	GraphToRemove->GetNodesOfClassEx<UPixel2DAnimStateTransitionNode>(AllAnimCompositeNodes);

	for (UPixel2DAnimStateNodeBase* Node : AllAnimCompositeNodes)
	{
		UEdGraph* BoundGraph = Node->GetBoundGraph();
		if (BoundGraph && BoundGraph->GetOuter()->IsA(UPixel2DAnimStateNodeBase::StaticClass()))
		{
			FPixel2DAnimBlueprintUtils::RemoveGraph(Blueprint, BoundGraph, EGraphRemoveFlags::None);
		}
	}

	// Handle sub anim state machines
	TArray<UPixel2DAnimGraphNode_StateMachineBase*> AllStateMachines;
	GraphToRemove->GetNodesOfClassEx<UPixel2DAnimGraphNode_StateMachine>(AllStateMachines);

	for (UPixel2DAnimGraphNode_StateMachineBase* Node : AllStateMachines)
	{
		UEdGraph* BoundGraph = Node->EditorStateMachineGraph;
		if (BoundGraph && BoundGraph->GetOuter()->IsA(UPixel2DAnimGraphNode_StateMachineBase::StaticClass()))
		{
			FPixel2DAnimBlueprintUtils::RemoveGraph(Blueprint, BoundGraph, EGraphRemoveFlags::None);
		}
	}

	GraphToRemove->GetSchema()->HandleGraphBeingDeleted(*GraphToRemove);

	GraphToRemove->Rename(nullptr, Blueprint->GetOuter(), REN_DoNotDirty | REN_DontCreateRedirectors);
	GraphToRemove->ClearFlags(RF_Standalone | RF_Public);
	GraphToRemove->RemoveFromRoot();

	if (Flags & EGraphRemoveFlags::MarkTransient)
	{
		GraphToRemove->SetFlags(RF_Transient);
	}

	GraphToRemove->MarkPendingKill();

	if (Flags & EGraphRemoveFlags::Recompile)
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	}
}