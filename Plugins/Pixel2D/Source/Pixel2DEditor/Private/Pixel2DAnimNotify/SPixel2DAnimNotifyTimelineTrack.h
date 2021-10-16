// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SPixel2DAnimNotifyTimeline.h"
#include "Types/SlateStructs.h"
#include "PaperFlipbook.h"

class FUICommandList;
class SHorizontalBox;

namespace FFlipbookUIConstants
{
	const float HandleWidth = 12.0f;
	const float FrameHeight = 48;
	const float HeightBeforeFrames = 16;
	const FMargin FramePadding(0.0f, 7.0f, 0.0f, 7.0f);
};

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifkKeyframeWidget

class SPixel2DAnimNotifkKeyframeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPixel2DAnimNotifkKeyframeWidget)
		: _SlateUnitsPerFrame(1)
		, _FlipbookBeingEdited(nullptr)
	{}

		SLATE_ATTRIBUTE(float, SlateUnitsPerFrame)
		SLATE_ATTRIBUTE(class UPaperFlipbook*, FlipbookBeingEdited)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, int32 InFrameIndex);

	// SWidget interface
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	// End of SWidget interface

protected:
	FReply KeyframeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FText GetKeyframeAssetName() const;
	FText GetKeyframeText() const;
	FText GetKeyframeTooltip() const;

	TSharedRef<SWidget> GenerateContextMenu();

	FOptionalSize GetFrameWidth() const;

	// Can return null
	const struct FPaperFlipbookKeyFrame* GetKeyFrameData() const;

protected:
	int32 FrameIndex;
	TAttribute<float> SlateUnitsPerFrame;
	TAttribute<class UPaperFlipbook*> FlipbookBeingEdited;
	TSharedPtr<FUICommandList> CommandList;
};

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifyTimelineTrack

class SPixel2DAnimNotifyTimelineTrack : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPixel2DAnimNotifyTimelineTrack)
		: _SlateUnitsPerFrame(1)
		, _FlipbookBeingEdited(nullptr)
	{}

		SLATE_ATTRIBUTE(float, SlateUnitsPerFrame)
		SLATE_ATTRIBUTE(class UPaperFlipbook*, FlipbookBeingEdited)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void Rebuild();

private:
	TAttribute<float> SlateUnitsPerFrame;
	TAttribute< class UPaperFlipbook* > FlipbookBeingEdited;

	TSharedPtr<SHorizontalBox> MainBoxPtr;

	float HandleWidth;

	TSharedPtr<FUICommandList> CommandList;

	// SWidget interface
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		return FReply::Unhandled();
	};
};
