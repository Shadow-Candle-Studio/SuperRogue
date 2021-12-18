// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionSGraphNode.h"




void TransitionSGraphNode::Construct(const FArguments& InArgs, UAnimStateTransitionNode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}
