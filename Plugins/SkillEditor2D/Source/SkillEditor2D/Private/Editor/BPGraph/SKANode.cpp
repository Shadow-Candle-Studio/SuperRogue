// Fill out your copyright notice in the Description page of Project Settings.


#include "SKANode.h"

void USKANode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input,FName("StateNodePin"),FName("In"));
	CreatePin(EGPD_Output,FName("StateNodePin"),FName("Out"));
}

void USKANode::AutowireNewNode(UEdGraphPin* FromPin)
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

TArray<UAnimStateTransitionNode*>& USKANode::GetTransitionNodes()
{
	return TransitionNodes;
}

