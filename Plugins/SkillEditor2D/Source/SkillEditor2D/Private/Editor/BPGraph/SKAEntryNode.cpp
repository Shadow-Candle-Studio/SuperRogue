// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/BPGraph/SKAEntryNode.h"

#include "Editor/BPGraph/SKAUEdGraphSchema.h"
#define LOCTEXT_NAMESPACE "SKAEntryNodeTest"

USKAEntryNode::USKAEntryNode()
{
	;
}

void USKAEntryNode::AllocateDefaultPins()
{
	
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, USKAUEdGraphSchema::PC_Exec, TEXT("Entry"));
}

FText USKAEntryNode::GetNodeTitle(ENodeTitleType::Type Title) const
{
	UEdGraph* Graph = GetGraph();
	if(IsValid(Graph))
		return FText::FromString(Graph->GetName());
	else
	{
		return FText::FromString("Graph nullptr!!");
	}
	
}

FText USKAEntryNode::GetTooltipText() const
{
	return LOCTEXT("StateMachine node entry ", "Entry point for test");
}



UEdGraphNode* USKAEntryNode::GetOutputNode() const
{
	if(Pins.Num() > 0 && Pins[0] != nullptr)
	{
		check(Pins[0]->LinkedTo.Num() <= 1);
		if(Pins[0]->LinkedTo.Num() > 0 && Pins[0]->LinkedTo[0]->GetOwningNode() != NULL)
		{
			return Pins[0]->LinkedTo[0]->GetOwningNode();
		}
	}
	return nullptr;
}

void SGraphSKAEntryNode::Construct(const FArguments& InArgs, USKAEntryNode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}
#undef LOCTEXT_NAMESPACE
