// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SPixel2DAnimNotifyTimeline.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Colors/SColorBlock.h"
#include "EditorStyleSet.h"
#include "PaperFlipbook.h"
#include "Pixel2DAnimGraphNode_AssetSprite.h"
#include "SPixel2DAnimNotifyTimelineTrack.h"
#include "SPixel2DAnimNotifyTimelineSlots.h"

#define LOCTEXT_NAMESPACE "SPixel2DAnimNotifyTimeline"

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifyTimeline

void SPixel2DAnimNotifyTimeline::Construct(const FArguments& InArgs, TSharedPtr<FUICommandList> InCommandList)
{
	EditedNode              = InArgs._EditedNode;
	OnSelectionChanged      = InArgs._OnSelectionChanged;
	OnMoveNotifyEvent       = InArgs._OnMoveNotifyEvent;
	EditedFlipbook          = EditedNode.Get()->GetAnimationAsset();
	CommandList             = InCommandList;

	SlateUnitsPerFrame = 120.0f;

	BackgroundPerFrameSlices = SNew(SHorizontalBox);

	TimelineTrack = SNew(SPixel2DAnimNotifyTimelineTrack)
		.SlateUnitsPerFrame(this, &SPixel2DAnimNotifyTimeline::GetSlateUnitsPerFrame)
		.FlipbookBeingEdited(EditedFlipbook.Get());

	TimelineNotifySlots = SNew(SPixel2DAnimNotifyTimelineSlots, CommandList)
		.EditedNode(EditedNode.Get())
		.OnMoveNotifyEvent(OnMoveNotifyEvent)
		.OnSelectionChanged(OnSelectionChanged);

	ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SScrollBox)
				.Orientation(Orient_Horizontal)
				.ScrollBarAlwaysVisible(true)
				.OnUserScrolled(this, &SPixel2DAnimNotifyTimeline::AnimationScrollBar_OnUserScrolled)
				+SScrollBox::Slot()
				[
					SNew(SOverlay)
					//Per-frame background
					+SOverlay::Slot()
					.VAlign(VAlign_Fill)
					[
						BackgroundPerFrameSlices.ToSharedRef()
					]
					// Flipbook header and track
					+SOverlay::Slot()
					[
						SNew(SBox)
						.HeightOverride(48)
						[
							TimelineTrack.ToSharedRef()
						]
					]

					// Empty flipbook instructions
					+SOverlay::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Visibility(this, &SPixel2DAnimNotifyTimeline::NoFramesWarningVisibility)
						.Text(LOCTEXT("EmptyTimelineInstruction", "Selected flipbook has no frames."))
					]

					// Notify slots
					+SOverlay::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						[
							TimelineNotifySlots.ToSharedRef()
						]
					]
				]
			]
		];

	UPaperFlipbook* Flipbook = EditedNode.Get()->GetAnimationAsset();
	NumKeyFramesFromLastRebuild = (Flipbook != nullptr) ? Flipbook->GetNumKeyFrames() : 0;
	NumFramesFromLastRebuild = (Flipbook != nullptr) ? Flipbook->GetNumFrames() : 0;
	RebuildPerFrameBG();
}

FReply SPixel2DAnimNotifyTimeline::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsControlDown())
	{
		const float DirectionScale = 0.08f;
		const float MinFrameSize = 16.0f;
		const float Direction = MouseEvent.GetWheelDelta();
		const float NewUnitsPerFrame = FMath::Max(MinFrameSize, SlateUnitsPerFrame * (1.0f + Direction * DirectionScale));
		SlateUnitsPerFrame = NewUnitsPerFrame;

		CheckForRebuild(/*bRebuildAll=*/ true);

		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SPixel2DAnimNotifyTimeline::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

void SPixel2DAnimNotifyTimeline::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	CheckForRebuild();
}

void SPixel2DAnimNotifyTimeline::CheckForRebuild(bool bRebuildAll)
{
	UPaperFlipbook* Flipbook = EditedNode.Get()->GetAnimationAsset();

	const int32 NewNumKeyframes = (Flipbook != nullptr) ? Flipbook->GetNumKeyFrames() : 0;
	if ((NewNumKeyframes != NumKeyFramesFromLastRebuild) || bRebuildAll)
	{
		NumKeyFramesFromLastRebuild = NewNumKeyframes;
		TimelineTrack->Rebuild();
		TimelineNotifySlots->Rebuild();
	}

	const int32 NewNumFrames = (Flipbook != nullptr) ? Flipbook->GetNumFrames() : 0;
	if ((NewNumFrames != NumFramesFromLastRebuild) || bRebuildAll)
	{
		NumFramesFromLastRebuild = NewNumFrames;
		RebuildPerFrameBG();
	}
}

EVisibility SPixel2DAnimNotifyTimeline::NoFramesWarningVisibility() const
{
	UPaperFlipbook* Flipbook = EditedNode.Get()->GetAnimationAsset();
	const int32 TotalNumFrames = (Flipbook != nullptr) ? Flipbook->GetNumFrames() : 0;
	return (TotalNumFrames == 0) ? EVisibility::Visible : EVisibility::Collapsed;
}

void SPixel2DAnimNotifyTimeline::RebuildPerFrameBG()
{
	const FLinearColor BackgroundColors[2] = { FLinearColor(1.0f, 1.0f, 1.0f, 0.05f), FLinearColor(0.0f, 0.0f, 0.0f, 0.05f) };

	BackgroundPerFrameSlices->ClearChildren();
	for (int32 FrameIndex = 0; FrameIndex < NumFramesFromLastRebuild; ++FrameIndex)
	{
		const FLinearColor& BackgroundColorForFrameIndex = BackgroundColors[FrameIndex & 1];

		BackgroundPerFrameSlices->AddSlot()
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(SlateUnitsPerFrame)
				[
					SNew(SColorBlock)
					.Color(BackgroundColorForFrameIndex)
				]
			];
	}
}

void SPixel2DAnimNotifyTimeline::AnimationScrollBar_OnUserScrolled(float ScrollOffset)
{
	AnimationScrollBarPosition = ScrollOffset;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
