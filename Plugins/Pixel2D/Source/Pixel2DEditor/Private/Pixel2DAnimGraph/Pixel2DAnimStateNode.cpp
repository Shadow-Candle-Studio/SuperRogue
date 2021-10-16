// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
Pixel2DAnimStateNode.cpp
=============================================================================*/

#include "Pixel2DAnimStateNode.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Pixel2DAnimBlueprintUtils.h"
#include "Pixel2DAnimStateTransitionNode.h"
#include "Pixel2DAnimStateGraph.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Pixel2DAnimGraphNode_StateResult.h"
#include "Pixel2DAnimStateGraphSchema.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnimStateNode"

/////////////////////////////////////////////////////
// UPixel2DAnimStateNode

UPixel2DAnimStateNode::UPixel2DAnimStateNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCanRenameNode = true;
	bAlwaysResetOnEntry = false;
}

void UPixel2DAnimStateNode::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Transition"), TEXT("In"));
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Transition"), TEXT("Out"));
}

void UPixel2DAnimStateNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	//@TODO: If the FromPin is a state, create a transition between us
	if (FromPin)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

FText UPixel2DAnimStateNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetStateName());
}

FText UPixel2DAnimStateNode::GetTooltipText() const
{
	return LOCTEXT("AnimStateNode_Tooltip", "This is a state");
}

void UPixel2DAnimStateNode::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == FName(TEXT("StateType")))
	{
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}


FString UPixel2DAnimStateNode::GetStateName() const
{
	return (BoundGraph != NULL) ? *(BoundGraph->GetName()) : TEXT("(null)");
}

void UPixel2DAnimStateNode::GetTransitionList(TArray<UPixel2DAnimStateTransitionNode*>& OutTransitions, bool bWantSortedList)
{
	// Normal transitions
	for (int32 LinkIndex = 0; LinkIndex < Pins[1]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[1]->LinkedTo[LinkIndex]->GetOwningNode();
		if (UPixel2DAnimStateTransitionNode* Transition = Cast<UPixel2DAnimStateTransitionNode>(TargetNode))
		{
			OutTransitions.Add(Transition);
		}
	}

	// Bidirectional transitions where we are the 'backwards' link
	for (int32 LinkIndex = 0; LinkIndex < Pins[0]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[0]->LinkedTo[LinkIndex]->GetOwningNode();
		if (UPixel2DAnimStateTransitionNode* Transition = Cast<UPixel2DAnimStateTransitionNode>(TargetNode))
		{
			if (Transition->Bidirectional)
			{
				OutTransitions.Add(Transition);
			}
		}
	}

	// Sort the transitions by priority order, lower numbers are higher priority
	if (bWantSortedList)
	{
		struct FCompareTransitionsByPriority
		{
			FORCEINLINE bool operator()(const UPixel2DAnimStateTransitionNode& A, const UPixel2DAnimStateTransitionNode& B) const
			{
				return A.PriorityOrder < B.PriorityOrder;
			}
		};

		OutTransitions.Sort(FCompareTransitionsByPriority());
	}
}


UEdGraphPin* UPixel2DAnimStateNode::GetInputPin() const
{
	return Pins[0];
}


UEdGraphPin* UPixel2DAnimStateNode::GetOutputPin() const
{
	return Pins[1];
}

UEdGraphPin* UPixel2DAnimStateNode::GetPoseSinkPinInsideState() const
{
	if (UPixel2DAnimStateGraph* StateGraph = Cast<UPixel2DAnimStateGraph>(BoundGraph))
	{
		return (StateGraph->MyResultNode != NULL) ? StateGraph->MyResultNode->FindPin(TEXT("Result")) : NULL;
	}
	else
	{
		return NULL;
	}
}

void UPixel2DAnimStateNode::PostPasteNode()
{
	// Find an interesting name, but try to keep the same if possible
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	FBlueprintEditorUtils::RenameGraphWithSuggestion(BoundGraph, NameValidator, GetStateName());
	Super::PostPasteNode();
}

void UPixel2DAnimStateNode::PostPlacedNewNode()
{
	// Create a new animation graph
	check(BoundGraph == NULL);
	BoundGraph = FBlueprintEditorUtils::CreateNewGraph(
		this,
		NAME_None,
		UPixel2DAnimStateGraph::StaticClass(),
		UPixel2DAnimStateGraphSchema::StaticClass());
	check(BoundGraph);

	// Find an interesting name
	TSharedPtr<INameValidatorInterface> NameValidator = FNameValidatorFactory::MakeValidator(this);
	FBlueprintEditorUtils::RenameGraphWithSuggestion(BoundGraph, NameValidator, TEXT("State"));

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

void UPixel2DAnimStateNode::DestroyNode()
{
	UEdGraph* GraphToRemove = BoundGraph;

	BoundGraph = NULL;
	Super::DestroyNode();

	if (GraphToRemove)
	{
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
		FPixel2DAnimBlueprintUtils::RemoveGraph(Blueprint, GraphToRemove, EGraphRemoveFlags::Recompile);
	}
}

#undef LOCTEXT_NAMESPACE
