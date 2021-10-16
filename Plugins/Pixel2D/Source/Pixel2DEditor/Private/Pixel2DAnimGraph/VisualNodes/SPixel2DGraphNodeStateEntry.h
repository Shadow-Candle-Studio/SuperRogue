// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "SGraphNode.h"

class SGraphPin;
class UPixel2DAnimStateEntryNode;

class SPixel2DGraphNodeStateEntry : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SPixel2DGraphNodeStateEntry) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UPixel2DAnimStateEntryNode* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	// End of SGraphNode interface

protected:
	FSlateColor GetBorderBackgroundColor() const;

	FText GetPreviewCornerText() const;
};
