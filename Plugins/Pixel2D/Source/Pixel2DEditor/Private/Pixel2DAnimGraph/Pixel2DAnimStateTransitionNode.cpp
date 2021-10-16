// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
Pixel2DAnimStateTransitionNode.cpp
=============================================================================*/

#include "Pixel2DAnimStateTransitionNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Pixel2DAnimBlueprintUtils.h"
#include "Pixel2DAnimInstance.h"
#include "Pixel2DAnimTransitionGraph.h"
#include "Pixel2DAnimTransitionSchema.h"
#include "Pixel2DAnimGraphNode_TransitionResult.h"
#include "Kismet2/CompilerResultsLog.h"
#include "EdGraphUtilities.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "UPixel2DAnimStateTransitionNode"

/////////////////////////////////////////////////////
// UPixel2DAnimStateTransitionNode

UPixel2DAnimStateTransitionNode::UPixel2DAnimStateTransitionNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	CrossfadeDuration = 0.2f;
	bSharedRules = false;
	SharedRulesGuid.Invalidate();
	bSharedCrossfade = false;
	SharedCrossfadeIdx = INDEX_NONE;
	SharedCrossfadeGuid.Invalidate();
	Bidirectional = false;
	PriorityOrder = 1;
}

void UPixel2DAnimStateTransitionNode::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
	Outputs->bHidden = true;
}

void UPixel2DAnimStateTransitionNode::PostPlacedNewNode()
{
	CreateBoundGraph();
}

void UPixel2DAnimStateTransitionNode::PostPasteNode()
{

	if (BoundGraph == NULL)
	{
		// fail-safe, create empty transition graph
		CreateBoundGraph();
	}

	Super::PostPasteNode();

	// We don't want to paste nodes in that aren't fully linked (transition nodes have fixed pins as they
	// really describle the connection between two other nodes). If we find one missing link, get rid of the node.
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->LinkedTo.Num() == 0)
		{
			DestroyNode();
			break;
		}
	}
}

FText UPixel2DAnimStateTransitionNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UPixel2DAnimStateNodeBase* PrevState = GetPreviousState();
	UPixel2DAnimStateNodeBase* NextState = GetNextState();

	if (!SharedRulesName.IsEmpty())
	{
		return FText::FromString(SharedRulesName);
	}
	else if ((PrevState != NULL) && (NextState != NULL))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("PrevState"), FText::FromString(PrevState->GetStateName()));
		Args.Add(TEXT("NextState"), FText::FromString(NextState->GetStateName()));

		return FText::Format(LOCTEXT("PrevStateToNewState", "{PrevState} to {NextState}"), Args);
	}
	else
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("BoundGraph"), (BoundGraph != NULL) ? FText::FromString(BoundGraph->GetName()) : LOCTEXT("Null", "(null)"));
		// @TODO: FText::Format() is slow, and we could benefit from caching 
		//        this off like we do for a lot of other nodes (but we have to
		//        make sure to invalidate the cached string at the appropriate 
		//        times).
		return FText::Format(LOCTEXT("TransitioNState", "Trans {BoundGraph}}"), Args);
	}
}

FText UPixel2DAnimStateTransitionNode::GetTooltipText() const
{
	return LOCTEXT("StateTransitionTooltip", "This is a state transition");
}

UPixel2DAnimStateNodeBase* UPixel2DAnimStateTransitionNode::GetPreviousState() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UPixel2DAnimStateNodeBase>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

UPixel2DAnimStateNodeBase* UPixel2DAnimStateTransitionNode::GetNextState() const
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UPixel2DAnimStateNodeBase>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return NULL;
	}
}

FLinearColor UPixel2DAnimStateTransitionNode::GetNodeTitleColor() const
{
	return FColorList::Red;
}

void UPixel2DAnimStateTransitionNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UPixel2DAnimStateTransitionNode::CreateConnections(UPixel2DAnimStateNodeBase* PreviousState, UPixel2DAnimStateNodeBase* NextState)
{
	// Previous to this
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	PreviousState->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(PreviousState->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	NextState->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(NextState->GetInputPin());
}

void UPixel2DAnimStateTransitionNode::PrepareForCopying()
{
	Super::PrepareForCopying();
	// move bound graph node here, so during copying it will be referenced
	// for shared nodes at least one of them has to be referencing it, so we will be fine
	BoundGraph->Rename(NULL, this, REN_DoNotDirty | REN_DontCreateRedirectors);
}

void UPixel2DAnimStateTransitionNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UPixel2DAnimStateTransitionNode, CrossfadeDuration))
	{
		PropagateCrossfadeSettings();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FString UPixel2DAnimStateTransitionNode::GetStateName() const
{
	return (BoundGraph != NULL) ? *(BoundGraph->GetName()) : TEXT("(null)");
}

void UPixel2DAnimStateTransitionNode::MakeRulesShareable(FString ShareName)
{
	bSharedRules = true;
	SharedRulesName = ShareName;
	SharedRulesGuid = FGuid::NewGuid();
}

void UPixel2DAnimStateTransitionNode::MakeCrossfadeShareable(FString ShareName)
{
	// Give us a unique idx. This remaps every SharedCrossfadeIdx in the graph (in case some were deleted)
	UEdGraph* CurrentGraph = GetGraph();

	SharedCrossfadeIdx = INDEX_NONE;
	TArray<int32> Remap;
	for (int32 idx = 0; idx < CurrentGraph->Nodes.Num(); idx++)
	{
		if (UPixel2DAnimStateTransitionNode* Node = Cast<UPixel2DAnimStateTransitionNode>(CurrentGraph->Nodes[idx]))
		{
			if (Node->SharedCrossfadeIdx != INDEX_NONE || Node == this)
			{
				Node->SharedCrossfadeIdx = Remap.AddUnique(Node->SharedCrossfadeIdx) + 1; // Remaps existing index to lowest index available
			}
		}
	}

	bSharedCrossfade = true;
	SharedCrossfadeName = ShareName;
	SharedCrossfadeGuid = FGuid::NewGuid();
}

void UPixel2DAnimStateTransitionNode::UnshareRules()
{
	bSharedRules = false;
	SharedRulesName.Empty();
	SharedRulesGuid.Invalidate();

	if ((BoundGraph == NULL) || IsBoundGraphShared())
	{
		BoundGraph = NULL;
		CreateBoundGraph();
	}
}

void UPixel2DAnimStateTransitionNode::UnshareCrossade()
{
	bSharedCrossfade = false;
	SharedCrossfadeIdx = INDEX_NONE;
	SharedCrossfadeName.Empty();
	SharedCrossfadeGuid.Invalidate();
}

void UPixel2DAnimStateTransitionNode::UseSharedRules(const UPixel2DAnimStateTransitionNode* Node)
{
	if (Node == this || Node == nullptr)
	{
		return;
	}

	FScopedTransaction Transaction(LOCTEXT("UseSharedRules", "Use Shared Rules"));

	Modify();

	UEdGraph* CurrentGraph = GetGraph();
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(CurrentGraph);

	UEdGraph* GraphToDelete = NULL;
	if ((BoundGraph != NULL) && !IsBoundGraphShared())
	{
		GraphToDelete = BoundGraph;
	}

	BoundGraph = Node->BoundGraph;
	bSharedRules = Node->bSharedRules;
	SharedRulesName = Node->SharedRulesName;
	SharedColor = Node->SharedColor;
	SharedRulesGuid = Node->SharedRulesGuid;

	if (GraphToDelete != NULL)
	{
		FPixel2DAnimBlueprintUtils::RemoveGraph(Blueprint, GraphToDelete);
	}

	// If this node has shared crossfade settings, and we currently dont... share with it automatically.
	// We'll see if this is actually helpful or just confusing. I think it might be a common operation
	// and this avoid having to manually select to share the rules and then share the crossfade settings.
	if ((SharedCrossfadeIdx == INDEX_NONE) && (Node->SharedCrossfadeIdx != INDEX_NONE))
	{
		UseSharedCrossfade(Node);
	}
}

void UPixel2DAnimStateTransitionNode::UseSharedCrossfade(const UPixel2DAnimStateTransitionNode* Node)
{
	if (Node == this || Node == nullptr)
	{
		return;
	}

	FScopedTransaction Transaction(LOCTEXT("UseSharedCrossfade", "Use Shared Crossfade"));

	Modify();

	bSharedCrossfade = Node->bSharedCrossfade;
	SharedCrossfadeName = Node->SharedCrossfadeName;
	SharedCrossfadeGuid = Node->SharedCrossfadeGuid;
	CopyCrossfadeSettings(Node);
}

void UPixel2DAnimStateTransitionNode::CopyCrossfadeSettings(const UPixel2DAnimStateTransitionNode* SrcNode)
{
	CrossfadeDuration = SrcNode->CrossfadeDuration;
	SharedCrossfadeIdx = SrcNode->SharedCrossfadeIdx;
	SharedCrossfadeName = SrcNode->SharedCrossfadeName;
	SharedCrossfadeGuid = SrcNode->SharedCrossfadeGuid;
}

void UPixel2DAnimStateTransitionNode::PropagateCrossfadeSettings()
{
	UEdGraph* CurrentGraph = GetGraph();
	for (int32 idx = 0; idx < CurrentGraph->Nodes.Num(); idx++)
	{
		if (UPixel2DAnimStateTransitionNode* Node = Cast<UPixel2DAnimStateTransitionNode>(CurrentGraph->Nodes[idx]))
		{
			if (Node->SharedCrossfadeIdx != INDEX_NONE && Node->SharedCrossfadeGuid == SharedCrossfadeGuid)
			{
				Node->Modify();
				Node->CopyCrossfadeSettings(this);
			}
		}
	}
}

bool UPixel2DAnimStateTransitionNode::IsReverseTrans(const UPixel2DAnimStateNodeBase* Node)
{
	return (Bidirectional && GetNextState() == Node);
}

void UPixel2DAnimStateTransitionNode::CreateBoundGraph()
{
	// Create a new animation graph
	check(BoundGraph == NULL);
	BoundGraph = FBlueprintEditorUtils::CreateNewGraph(this, NAME_None, UPixel2DAnimTransitionGraph::StaticClass(), UPixel2DAnimTransitionSchema::StaticClass());
	check(BoundGraph);

	// Find an interesting name
	FEdGraphUtilities::RenameGraphToNameOrCloseToName(BoundGraph, TEXT("Transition"));

	// Initialize the anim graph
	const UEdGraphSchema* Schema = BoundGraph->GetSchema();
	Schema->CreateDefaultNodesForGraph(*BoundGraph);

	// Add the new graph as a child of our parent graph
	UEdGraph* ParentGraph = GetGraph();

	if (ParentGraph->SubGraphs.Find(BoundGraph) == INDEX_NONE)
	{
		ParentGraph->SubGraphs.Add(BoundGraph);
	}
}

void UPixel2DAnimStateTransitionNode::DestroyNode()
{
	// BoundGraph may be shared with another graph, if so, don't remove it here
	UEdGraph* GraphToRemove = IsBoundGraphShared() ? NULL : GetBoundGraph();

	BoundGraph = NULL;
	Super::DestroyNode();

	if (GraphToRemove)
	{
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
		FPixel2DAnimBlueprintUtils::RemoveGraph(Blueprint, GraphToRemove, EGraphRemoveFlags::Recompile);
	}
}

/** Returns true if this nodes BoundGraph is shared with another node in the parent graph */
bool UPixel2DAnimStateTransitionNode::IsBoundGraphShared()
{
	if (BoundGraph)
	{
		//@TODO: O(N) search
		UEdGraph* ParentGraph = GetGraph();
		for (int32 NodeIdx = 0; NodeIdx < ParentGraph->Nodes.Num(); NodeIdx++)
		{
			UPixel2DAnimStateNodeBase* AnimNode = Cast<UPixel2DAnimStateNodeBase>(ParentGraph->Nodes[NodeIdx]);
			if ((AnimNode != NULL) && (AnimNode != this) && (AnimNode->GetBoundGraph() == BoundGraph))
			{
				return true;
			}
		}
	}

	return false;
}

void UPixel2DAnimStateTransitionNode::ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);

	if (UPixel2DAnimTransitionGraph* TransGraph = Cast<UPixel2DAnimTransitionGraph>(BoundGraph))
	{
		UPixel2DAnimGraphNode_TransitionResult* ResultNode = TransGraph->GetResultNode();
		check(ResultNode);

		UEdGraphPin* BoolResultPin = ResultNode->Pins[0];
		if ((BoolResultPin->LinkedTo.Num() == 0) && (BoolResultPin->DefaultValue.ToBool() == false))
		{
			// check for native transition rule before warning
			bool bHasNative = false;
			UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
			if (Blueprint && Blueprint->ParentClass)
			{
				UPixel2DAnimInstance* AnimInstance = CastChecked<UPixel2DAnimInstance>(Blueprint->ParentClass->GetDefaultObject());
				if (AnimInstance)
				{
					UEdGraph* ParentGraph = GetGraph();
					UPixel2DAnimStateNodeBase* PrevState = GetPreviousState();
					UPixel2DAnimStateNodeBase* NextState = GetNextState();
					if (PrevState != nullptr && NextState != nullptr && ParentGraph != nullptr)
					{
						FName FunctionName;
						// REVISIT
						bHasNative = false; //AnimInstance->HasNativeTransitionBinding(ParentGraph->GetFName(), FName(*PrevState->GetStateName()), FName(*NextState->GetStateName()), FunctionName);
					}
				}
			}

			if (!bHasNative && !bAutomaticRuleBasedOnSequencePlayerInState)
			{
				MessageLog.Warning(TEXT("@@ will never be taken, please connect something to @@"), this, BoolResultPin);
			}
		}
	}
	else
	{
		MessageLog.Error(TEXT("@@ contains an invalid or NULL BoundGraph.  Please delete and recreate the transition."), this);
	}
}

#undef LOCTEXT_NAMESPACE
