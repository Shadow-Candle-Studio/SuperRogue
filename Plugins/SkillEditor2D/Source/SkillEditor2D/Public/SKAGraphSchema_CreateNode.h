// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "SKAGraphSchema_CreateNode.generated.h"

/**
 * 
 */
USTRUCT()
struct SKILLEDITOR2D_API FSKAGraphSchema_CreateNode : public FEdGraphSchemaAction
{
	GENERATED_BODY()
	FSKAGraphSchema_CreateNode(){};
	FSKAGraphSchema_CreateNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{};
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	static UEdGraphNode* CreateStateNode(class UEdGraph* ParentGraph, const FVector2D Location);
	
};
