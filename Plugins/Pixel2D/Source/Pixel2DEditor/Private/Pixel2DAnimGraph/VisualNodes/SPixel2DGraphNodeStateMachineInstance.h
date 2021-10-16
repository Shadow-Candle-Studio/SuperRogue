// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KismetNodes/SGraphNodeK2Composite.h"

class UEdGraph;

class SPixel2DGraphNodeStateMachineInstance : public SGraphNodeK2Composite
{
public:
	SLATE_BEGIN_ARGS(SPixel2DGraphNodeStateMachineInstance) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UPixel2DAnimGraphNode_StateMachineBase* InNode);
protected:
	// SGraphNodeK2Composite interface
	virtual UEdGraph* GetInnerGraph() const override;
	// End of SGraphNodeK2Composite interface
};
