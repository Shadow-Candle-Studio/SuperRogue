// Fill out your copyright notice in the Description page of Project Settings.


#include "SKAGraphSchema_CreateNode.h"

UEdGraphNode* FSKAGraphSchema_CreateNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	return CreateStateNode(ParentGraph, Location);
}

UEdGraphNode* FSKAGraphSchema_CreateNode::CreateStateNode(UEdGraph* ParentGraph, const FVector2D Location)
{
	if (ParentGraph == NULL) return NULL;
	// make a temp node
	UEdGraphNode* ResultNode = NewObject<UEdGraphNode>(ParentGraph);
	ParentGraph->Modify();
	ResultNode->SetFlags(RF_Transactional);
	// set outer to be the graph so it doesn't go away
	ResultNode->Rename(NULL, ParentGraph, REN_NonTransactional);
	ParentGraph->AddNode(ResultNode, true);
	ResultNode->CreateNewGuid();
	ResultNode->NodePosX = Location.X;
	ResultNode->NodePosY = Location.Y;
	//ResultNode->SnapToGrid(AE_SNAP_GRID);
	// setup pins after placing node
	ResultNode->AllocateDefaultPins();
	return ResultNode;
}
