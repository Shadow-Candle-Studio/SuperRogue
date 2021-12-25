// Fill out your copyright notice in the Description page of Project Settings.



#include "Editor/BPGraph/SKAUEdGraphSchema.h"

#include "Editor/BPGraph/SKAGraphSchema_CreateNode.h"
#define LOCTEXT_NAMESPACE "USKAUEdGraphSchema"


USKAUEdGraphSchema::
USKAUEdGraphSchema(const FObjectInitializer& ObjectInitializer):
Super(ObjectInitializer)
{
	;
}

void USKAUEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	Super::GetGraphContextActions(ContextMenuBuilder);
	// Add action
	FText Category = LOCTEXT("AddStateMachine", "Add State Node");
	FText MenuDesc = LOCTEXT("MenuDesc", "State");
	FText ToolTip = LOCTEXT("StateToolTip", "A State Node");
	TSharedPtr<FSKAGraphSchema_CreateNode> NewStateNode(new FSKAGraphSchema_CreateNode(Category, MenuDesc, ToolTip, 0));
	ContextMenuBuilder.AddAction(NewStateNode);
}

void USKAUEdGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetContextMenuActions(Menu, Context);
}

void USKAUEdGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	Super::CreateDefaultNodesForGraph(Graph);
}

const FPinConnectionResponse USKAUEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	return Super::CanCreateConnection(A, B);
}

bool USKAUEdGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	return Super::TryCreateConnection(A, B);
}

bool USKAUEdGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	return Super::CreateAutomaticConversionNodeAndConnections(A, B);
}

const FName USKAUEdGraphSchema::PC_Exec(TEXT("exec"));










#undef LOCTEXT_NAMESPACE