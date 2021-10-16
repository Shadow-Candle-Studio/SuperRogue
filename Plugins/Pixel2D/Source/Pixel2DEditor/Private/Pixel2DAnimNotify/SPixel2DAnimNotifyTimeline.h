// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Layout/Visibility.h"
#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Pixel2DAnimGraphNode_AssetSprite.h"
#include "Core/Public/Delegates/Delegate.h"

class FPaintArgs;
class FSlateWindowElementList;
class FUICommandList;
class SHorizontalBox;
class SPixel2DAnimNotifyTimelineSlots;
class SPixel2DAnimNotifyTimelineTrack;

// Called when the selection changes
DECLARE_DELEGATE_TwoParams(FOnPixel2DAnimNotifySelectionChanged, int32, int32);
DECLARE_DELEGATE_OneParam(FOnPixel2DAnimNotifyNotifyEventMoved, int32);

class SPixel2DAnimNotifyTimeline : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPixel2DAnimNotifyTimeline)
		: _EditedNode(nullptr)
	{}

		SLATE_ATTRIBUTE(class UPixel2DAnimGraphNode_AssetSprite*, EditedNode)
		SLATE_EVENT(FOnPixel2DAnimNotifySelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FOnPixel2DAnimNotifyNotifyEventMoved, OnMoveNotifyEvent)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<FUICommandList> InCommandList);

	// SWidget interface
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	// End of SWidget interface

private:
	void RebuildPerFrameBG();

	EVisibility NoFramesWarningVisibility() const;
	void CheckForRebuild(bool bRebuildAll = false);

	float GetSlateUnitsPerFrame() const { return SlateUnitsPerFrame; }

	void AnimationScrollBar_OnUserScrolled(float ScrollOffset);
private:
	TSharedPtr<SHorizontalBox> BackgroundPerFrameSlices;
	TSharedPtr<class SPixel2DAnimNotifyTimelineTrack> TimelineTrack;
	TSharedPtr<class SPixel2DAnimNotifyTimelineSlots> TimelineNotifySlots;

	TAttribute<class UPixel2DAnimGraphNode_AssetSprite*> EditedNode;
	TAttribute<class UPaperFlipbook*> EditedFlipbook;
	TSharedPtr<FUICommandList> CommandList;
	FOnPixel2DAnimNotifySelectionChanged OnSelectionChanged;
	FOnPixel2DAnimNotifyNotifyEventMoved OnMoveNotifyEvent;
	float SlateUnitsPerFrame;
	float AnimationScrollBarPosition;

	// Observer values (used to detect if the widget needs to be rebuilt)
	int32 NumFramesFromLastRebuild;
	int32 NumKeyFramesFromLastRebuild;
};
