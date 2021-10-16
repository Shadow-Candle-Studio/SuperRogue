// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "SPixel2DGraphNode.h"

class SPixel2DGraphNodeResult : public SPixel2DGraphNode
{
public:
	SLATE_BEGIN_ARGS(SPixel2DGraphNodeResult) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UPixel2DAnimGraphNode_Base* InNode);

protected:
	// SGraphNode interface
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
	// End of SGraphNode interface
};
