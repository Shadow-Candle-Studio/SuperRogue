// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SNodePanel.h"
#include "SGraphNode.h"
#include "SGraphPin.h"

class SToolTip;
class UAnimStateConduitNode;
class UPixel2DAnimStateNodeBase;

//
// Forward declarations.
//
class UPixel2DAnimStateNodeBase;
class UAnimStateConduitNode;
class UPixel2DAnimStateEntryNode;

class SPixel2DGraphNodeState : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SPixel2DGraphNodeState) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UPixel2DAnimStateNodeBase* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// End of SGraphNode interface

protected:
	FSlateColor GetBorderBackgroundColor() const;

	virtual FText GetPreviewCornerText() const;
	virtual const FSlateBrush* GetNameIcon() const;
};


class SPixel2DGraphNodeConduit : public SPixel2DGraphNodeState
{
public:
	SLATE_BEGIN_ARGS(SPixel2DGraphNodeConduit) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UAnimStateConduitNode* InNode);

protected:
	virtual FText GetPreviewCornerText() const override;
	virtual const FSlateBrush* GetNameIcon() const override;
};
