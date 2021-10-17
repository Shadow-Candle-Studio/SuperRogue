// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SPixel2DGraphNodeResult.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Pixel2DAnimGraphNode_Base.h"
#include "Pixel2DStyle.h"

/////////////////////////////////////////////////////
// SPixel2DGraphNodeResult

void SPixel2DGraphNodeResult::Construct(const FArguments& InArgs, UPixel2DAnimGraphNode_Base* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();

	SPixel2DGraphNode::Construct(SPixel2DGraphNode::FArguments(), InNode);
}

TSharedRef<SWidget> SPixel2DGraphNodeResult::CreateNodeContentArea()
{
	return SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.Padding(FMargin(0, 3))
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.FillWidth(1.0f)
			 [
				// LEFT
				 SAssignNew(LeftNodeBox, SVerticalBox)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(FPixel2DStyle::Get()->GetBrush("AnimGraph.ResultNodeIcon"))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			.Padding(FMargin(15, 3))
			[
				// RIGHT
				SAssignNew(RightNodeBox, SVerticalBox)
			]
		];
}
