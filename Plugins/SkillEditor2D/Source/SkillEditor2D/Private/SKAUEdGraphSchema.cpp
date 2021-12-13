// Fill out your copyright notice in the Description page of Project Settings.


#include "SKAUEdGraphSchema.h"

#include "SKAGraphSchema_CreateNode.h"
#define LOCTEXT_NAMESPACE "USKAUEdGraphSchema"
USKAUEdGraphSchema::
USKAUEdGraphSchema(const FObjectInitializer& ObjectInitializer):
Super(ObjectInitializer)
{
	;
}

void USKAUEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Add action
	FText Category = LOCTEXT("AddStateMachine", "Add State Node");
	FText MenuDesc = LOCTEXT("MenuDesc", "State");
	FText ToolTip = LOCTEXT("StateToolTip", "A State Node");
	TSharedPtr<FSKAGraphSchema_CreateNode> NewStateNode(new FSKAGraphSchema_CreateNode(Category, MenuDesc, ToolTip, 0));
	ContextMenuBuilder.AddAction(NewStateNode);
}












#undef LOCTEXT_NAMESPACE