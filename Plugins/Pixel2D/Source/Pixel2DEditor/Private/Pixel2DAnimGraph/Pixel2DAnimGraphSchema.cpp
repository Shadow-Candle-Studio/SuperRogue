// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
Pixel2DAnimGraphSchema.cpp
=============================================================================*/

#include "Pixel2DAnimGraphSchema.h"
#include "Pixel2DAnimAsset.h"
#include "Animation/AnimBlueprint.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "K2Node.h"
#include "ToolMenus/Public/ToolMenu.h"
#include "EdGraphSchema_K2_Actions.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Animation/AnimSequence.h"
#include "Pixel2DAnimStateNode.h"
#include "Animation/AnimNodeBase.h"
#include "Pixel2DAnimGraphNode_Base.h"
#include "Pixel2DAnimGraphNode_AssetPlayerBase.h"
#include "Animation/PoseAsset.h"
#include "AnimGraphCommands.h"
#include "K2Node_Knot.h"
#include "ScopedTransaction.h"
#include "PaperFlipbook.h"

#include "Pixel2DAnimGraphNode_Root.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnimGraphSchema"

/////////////////////////////////////////////////////
// UPixel2DAnimGraphSchema

UPixel2DAnimGraphSchema::UPixel2DAnimGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PN_SequenceName = TEXT("Sequence");

	NAME_NeverAsPin = TEXT("NeverAsPin");
	NAME_PinHiddenByDefault = TEXT("PinHiddenByDefault");
	NAME_PinShownByDefault = TEXT("PinShownByDefault");
	NAME_AlwaysAsPin = TEXT("AlwaysAsPin");
	NAME_OnEvaluate = TEXT("OnEvaluate");
	NAME_CustomizeProperty = TEXT("CustomizeProperty");
	DefaultEvaluationHandlerName = TEXT("EvaluateGraphExposedInputs");
}

FLinearColor UPixel2DAnimGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	const bool bAdditive = PinType.PinSubCategory == TEXT("Additive");
	if (UPixel2DAnimGraphSchema::IsLocalSpacePosePin(PinType))
	{
		if (bAdditive)
		{
			return FLinearColor(0.12, 0.60, 0.10);
		}
		else
		{
			return FLinearColor::White;
		}
	}

	return Super::GetPinTypeColor(PinType);
}

EGraphType UPixel2DAnimGraphSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_Animation;
}

void UPixel2DAnimGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	//// Create the result node
	FGraphNodeCreator<UPixel2DAnimGraphNode_Root> NodeCreator(Graph);
	UPixel2DAnimGraphNode_Root* ResultSinkNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(ResultSinkNode, FNodeMetadata::DefaultGraphNode);
}

void UPixel2DAnimGraphSchema::HandleGraphBeingDeleted(UEdGraph& GraphBeingRemoved) const
{
	if (UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(&GraphBeingRemoved))
	{
		// Look for state nodes that reference this graph
		TArray<UPixel2DAnimStateNodeBase*> StateNodes;
		FBlueprintEditorUtils::GetAllNodesOfClassEx<UPixel2DAnimStateNode>(Blueprint, StateNodes);

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

bool UPixel2DAnimGraphSchema::IsPosePin(const FEdGraphPinType& PinType)
{
	return IsLocalSpacePosePin(PinType);
}

bool UPixel2DAnimGraphSchema::IsLocalSpacePosePin(const FEdGraphPinType& PinType)
{
	const UPixel2DAnimGraphSchema* Schema = GetDefault<UPixel2DAnimGraphSchema>();

	UScriptStruct* PoseLinkStruct = FPixel2DPoseLink::StaticStruct();
	return (PinType.PinCategory == Schema->PC_Struct) && (PinType.PinSubCategoryObject == PoseLinkStruct);
}

bool UPixel2DAnimGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdGraphPin* OutputPin = nullptr;
	UEdGraphPin* InputPin = nullptr;

	if (A->Direction == EEdGraphPinDirection::EGPD_Output)
	{
		OutputPin = A;
		InputPin = B;
	}
	else
	{
		OutputPin = B;
		InputPin = A;
	}
	check(OutputPin && InputPin);

	UEdGraphNode* OutputNode = OutputPin->GetOwningNode();

	if (UK2Node_Knot* RerouteNode = Cast<UK2Node_Knot>(OutputNode))
	{
		// Double check this is our "exec"-like line
		bool bOutputIsPose = IsPosePin(OutputPin->PinType);
		bool bInputIsPose = IsPosePin(InputPin->PinType);
		bool bHavePosePin = bOutputIsPose || bInputIsPose;
		bool bHaveWildPin = InputPin->PinType.PinCategory == PC_Wildcard || OutputPin->PinType.PinCategory == PC_Wildcard;

		if ((bOutputIsPose && bInputIsPose) || (bHavePosePin && bHaveWildPin))
		{
			// Ok this is a valid exec-like line, we need to kill any connections already on the output pin
			OutputPin->BreakAllPinLinks();
		}
	}

	return Super::TryCreateConnection(A, B);
}

const FPinConnectionResponse UPixel2DAnimGraphSchema::DetermineConnectionResponseOfCompatibleTypedPins(const UEdGraphPin* PinA, const UEdGraphPin* PinB, const UEdGraphPin* InputPin, const UEdGraphPin* OutputPin) const
{
	//// Enforce a tree hierarchy; where poses can only have one output (parent) connection
	if (IsPosePin(OutputPin->PinType) && IsPosePin(InputPin->PinType))
	{
		if ((OutputPin->LinkedTo.Num() > 0) || (InputPin->LinkedTo.Num() > 0))
		{
			const ECanCreateConnectionResponse ReplyBreakOutputs = CONNECT_RESPONSE_BREAK_OTHERS_AB;
		   return FPinConnectionResponse(ReplyBreakOutputs, TEXT("Replace existing connections"));
		}
	}

	// Fall back to standard K2 rules
	return Super::DetermineConnectionResponseOfCompatibleTypedPins(PinA, PinB, InputPin, OutputPin);
}

bool UPixel2DAnimGraphSchema::ArePinsCompatible(const UEdGraphPin* PinA, const UEdGraphPin* PinB, const UClass* CallingContext, bool bIgnoreArray) const
{
	// both are pose pin, but doesn't match type, then return false;
	if (IsPosePin(PinA->PinType) && IsPosePin(PinB->PinType) && IsLocalSpacePosePin(PinA->PinType) != IsLocalSpacePosePin(PinB->PinType))
	{
		return false;
	}

	// Disallow pose pins connecting to wildcards (apart from reroute nodes)
	if (IsPosePin(PinA->PinType) && PinB->PinType.PinCategory == PC_Wildcard)
	{
		return Cast<UK2Node_Knot>(PinB->GetOwningNode()) != nullptr;
	}
	else if (IsPosePin(PinB->PinType) && PinA->PinType.PinCategory == PC_Wildcard)
	{
		return Cast<UK2Node_Knot>(PinA->GetOwningNode()) != nullptr;
	}

	return Super::ArePinsCompatible(PinA, PinB, CallingContext, bIgnoreArray);
}

bool UPixel2DAnimGraphSchema::DoesSupportAnimNotifyActions() const
{
	// Don't offer notify items in anim graph
	return false;
}

bool UPixel2DAnimGraphSchema::SearchForAutocastFunction(const UEdGraphPin* OutputPin, const UEdGraphPin* InputPin, FName& TargetFunction, /*out*/ UClass*& FunctionOwner) const
{
	return Super::SearchForAutocastFunction(OutputPin, InputPin, TargetFunction, FunctionOwner);
}

bool UPixel2DAnimGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	// Determine which pin is an input and which pin is an output
	UEdGraphPin* InputPin = NULL;
	UEdGraphPin* OutputPin = NULL;
	if (!CategorizePinsByDirection(PinA, PinB, /*out*/ InputPin, /*out*/ OutputPin))
	{
		return false;
	}

	{
		// Give the regular conversions a shot
		return Super::CreateAutomaticConversionNodeAndConnections(PinA, PinB);
	}
}

void UPixel2DAnimGraphSchema::SpawnNodeFromAsset(UPaperFlipbook* Asset, const FVector2D& GraphPosition, UEdGraph* Graph, UEdGraphPin* PinIfAvailable)
{
	check(Graph);
	check(Graph->GetSchema()->IsA(UPixel2DAnimGraphSchema::StaticClass()));
	check(Asset);

	UPixel2DAnimBlueprint* AnimBlueprint = Cast<UPixel2DAnimBlueprint>(FBlueprintEditorUtils::FindBlueprintForGraph(Graph));

	const bool bTypeMatch = (PinIfAvailable == NULL) || UPixel2DAnimGraphSchema::IsLocalSpacePosePin(PinIfAvailable->PinType);
	const bool bDirectionMatch = (PinIfAvailable == NULL) || (PinIfAvailable->Direction == EGPD_Input);

	if (bTypeMatch && bDirectionMatch)
	{
		FEdGraphSchemaAction_K2NewNode Action;

		UClass* NewNodeClass = GetNodeClassForAsset(Asset->GetClass());

		if (NewNodeClass)
		{
			check(NewNodeClass->IsChildOf(UPixel2DAnimGraphNode_AssetPlayerBase::StaticClass()));

			UPixel2DAnimGraphNode_AssetPlayerBase* NewNode = NewObject<UPixel2DAnimGraphNode_AssetPlayerBase>(GetTransientPackage(), NewNodeClass);
			NewNode->SetAnimationAsset(Asset);
			Action.NodeTemplate = NewNode;

			Action.PerformAction(Graph, PinIfAvailable, GraphPosition);
		}
	}
}

void UPixel2DAnimGraphSchema::UpdateNodeWithAsset(UK2Node* K2Node, UPaperFlipbook* Asset)
{
	// REVISIT
}

void UPixel2DAnimGraphSchema::DroppedAssetsOnGraph(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraph* Graph) const
{
	UPaperFlipbook* Asset = FAssetData::GetFirstAsset<UPaperFlipbook>(Assets);
	if ((Asset != NULL) && (Graph != NULL))
	{
		SpawnNodeFromAsset(Asset, GraphPosition, Graph, NULL);
	}
}

void UPixel2DAnimGraphSchema::DroppedAssetsOnNode(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraphNode* Node) const
{
	UPaperFlipbook* Asset = FAssetData::GetFirstAsset<UPaperFlipbook>(Assets);
	UK2Node* K2Node = Cast<UK2Node>(Node);
	if ((Asset != NULL) && (K2Node != NULL))
	{
		UpdateNodeWithAsset(K2Node, Asset);
	}
}

void UPixel2DAnimGraphSchema::DroppedAssetsOnPin(const TArray<FAssetData>& Assets, const FVector2D& GraphPosition, UEdGraphPin* Pin) const
{
	UPaperFlipbook* Asset = FAssetData::GetFirstAsset<UPaperFlipbook>(Assets);
	if ((Asset != NULL) && (Pin != NULL))
	{
		SpawnNodeFromAsset(Asset, GraphPosition, Pin->GetOwningNode()->GetGraph(), Pin);
	}
}

void UPixel2DAnimGraphSchema::GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	Super::GetContextMenuActions(Menu,Context);
}

FText UPixel2DAnimGraphSchema::GetPinDisplayName(const UEdGraphPin* Pin) const
{
	check(Pin != NULL);

	FText DisplayName = Super::GetPinDisplayName(Pin);

	if (UAnimGraphNode_Base* Node = Cast<UAnimGraphNode_Base>(Pin->GetOwningNode()))
	{
		FString ProcessedDisplayName = DisplayName.ToString();
		Node->PostProcessPinName(Pin, ProcessedDisplayName);
		DisplayName = FText::FromString(ProcessedDisplayName);
	}

	return DisplayName;
}

#undef LOCTEXT_NAMESPACE
