// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SPixel2DGraphNodeStateEntry.h"
#include "Pixel2DAnimStateEntryNode.h"
#include "Widgets/SBoxPanel.h"
#include "SGraphPin.h"

/////////////////////////////////////////////////////
// SPixel2DGraphNodeStateEntry

void SPixel2DGraphNodeStateEntry::Construct(const FArguments& InArgs, UPixel2DAnimStateEntryNode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

FSlateColor SPixel2DGraphNodeStateEntry::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return InactiveStateColor;
}

void SPixel2DGraphNodeStateEntry::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
				.Padding(0)
				.BorderBackgroundColor(this, &SPixel2DGraphNodeStateEntry::GetBorderBackgroundColor)
				[
					SNew(SOverlay)

					// PIN AREA
					+SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.Padding(10.0f)
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]
				]
			];

	CreatePinWidgets();
}

void SPixel2DGraphNodeStateEntry::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);
}

FText SPixel2DGraphNodeStateEntry::GetPreviewCornerText() const
{
	return NSLOCTEXT("SPixel2DGraphNodeStateEntry", "CornerTextDescription", "Entry point for state machine");
}
