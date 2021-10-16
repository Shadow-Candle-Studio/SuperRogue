// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraph.h"
#include "Pixel2DAnimStateEntryNode.h"
#include "Pixel2DAnimStateMachineGraph.generated.h"

UCLASS(MinimalAPI)
class UPixel2DAnimStateMachineGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()

	// Entry node within the state machine
	UPROPERTY()
	class UPixel2DAnimStateEntryNode* EntryNode;

	// Parent instance node
	UPROPERTY()
	class UPixel2DAnimGraphNode_StateMachineBase* OwnerAnimGraphNode;
};

