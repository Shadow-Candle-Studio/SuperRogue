// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SPixel2DAnimNotifyTimelineTrack.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/GenericCommands.h"
#include "Pixel2DAnimNotifyCommands.h"
#include "PaperSprite.h"

#define LOCTEXT_NAMESPACE "AnimNotifyEditor"

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifkKeyframeWidget

TSharedRef<SWidget> SPixel2DAnimNotifkKeyframeWidget::GenerateContextMenu()
{
	const FPixel2DAnimNotifyCommands& Commands = FPixel2DAnimNotifyCommands::Get();

	FMenuBuilder MenuBuilder(true, CommandList);
	{
		FNumberFormattingOptions NoCommas;
		NoCommas.UseGrouping = false;

		const FText KeyframeSectionTitle = FText::Format(LOCTEXT("KeyframeXActionsSectionHeader", "Keyframe #{0} Actions"), FText::AsNumber(FrameIndex, &NoCommas));
		MenuBuilder.BeginSection("KeyframeActions", KeyframeSectionTitle);
		MenuBuilder.EndSection();
	}

	return MenuBuilder.MakeWidget();
}

void SPixel2DAnimNotifkKeyframeWidget::Construct(const FArguments& InArgs, int32 InFrameIndex)
{
	FrameIndex = InFrameIndex;

	const FPixel2DAnimNotifyCommands& Commands = FPixel2DAnimNotifyCommands::Get();
	CommandList = MakeShareable(new FUICommandList);

	SlateUnitsPerFrame = InArgs._SlateUnitsPerFrame;
	FlipbookBeingEdited = InArgs._FlipbookBeingEdited;

	// Color each region based on whether a sprite has been set or not for it
	const auto BorderColorDelegate = [](TAttribute<UPaperFlipbook*> ThisFlipbookPtr, int32 TestIndex) -> FSlateColor
	{
		UPaperFlipbook* FlipbookPtr = ThisFlipbookPtr.Get();
		const bool bFrameValid = (FlipbookPtr != nullptr) && (FlipbookPtr->GetSpriteAtFrame(TestIndex) != nullptr);
		return bFrameValid ? FLinearColor::White : FLinearColor::Black;
	};

	ChildSlot
	[
		SNew(SOverlay)
		+SOverlay::Slot()
		[
			SNew(SBox)
			.Padding(FFlipbookUIConstants::FramePadding)
			.WidthOverride(this, &SPixel2DAnimNotifkKeyframeWidget::GetFrameWidth)
			[
				SNew(SBorder)
				.BorderBackgroundColor_Static(BorderColorDelegate, FlipbookBeingEdited, FrameIndex)
				.OnMouseButtonUp(this, &SPixel2DAnimNotifkKeyframeWidget::KeyframeOnMouseButtonUp)
				.ToolTipText(this, &SPixel2DAnimNotifkKeyframeWidget::GetKeyframeTooltip)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.ColorAndOpacity(FLinearColor::Black)
					.Text(this, &SPixel2DAnimNotifkKeyframeWidget::GetKeyframeText)
				]
			]
		]
	];
}

FReply SPixel2DAnimNotifkKeyframeWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}

FReply SPixel2DAnimNotifkKeyframeWidget::KeyframeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		TSharedRef<SWidget> MenuContents = GenerateContextMenu();
		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContents, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

// Can return null
const FPaperFlipbookKeyFrame* SPixel2DAnimNotifkKeyframeWidget::GetKeyFrameData() const
{
	UPaperFlipbook* Flipbook = FlipbookBeingEdited.Get();
	if ((Flipbook != nullptr) && Flipbook->IsValidKeyFrameIndex(FrameIndex))
	{
		return &(Flipbook->GetKeyFrameChecked(FrameIndex));
	}

	return nullptr;
}

FText SPixel2DAnimNotifkKeyframeWidget::GetKeyframeAssetName() const
{
	if (const FPaperFlipbookKeyFrame* KeyFrame = GetKeyFrameData())
	{
		const FText SpriteLine = (KeyFrame->Sprite != nullptr) ? FText::FromString(KeyFrame->Sprite->GetName()) : LOCTEXT("NoSprite", "(none)");
		return FText::Format(LOCTEXT("KeyFrameAssetName", "Current Asset: {0}"), SpriteLine);
	}
	else
	{
		return LOCTEXT("KeyFrameAssetName_None", "Current Asset: (none)");
	}
}

FText SPixel2DAnimNotifkKeyframeWidget::GetKeyframeText() const
{
	if (const FPaperFlipbookKeyFrame* KeyFrame = GetKeyFrameData())
	{
		if (KeyFrame->Sprite != nullptr)
		{
			return FText::AsCultureInvariant(KeyFrame->Sprite->GetName());
		}
	}

	return FText::GetEmpty();
}

FText SPixel2DAnimNotifkKeyframeWidget::GetKeyframeTooltip() const
{
	if (const FPaperFlipbookKeyFrame* KeyFrame = GetKeyFrameData())
	{
		const FText SpriteLine = (KeyFrame->Sprite != nullptr) ? FText::FromString(KeyFrame->Sprite->GetName()) : LOCTEXT("NoSprite", "(none)");

		const FText FramesText = (KeyFrame->FrameRun == 1) ? LOCTEXT("SingularFrames", "frame") : LOCTEXT("PluralFrames", "frames");

		return FText::Format(LOCTEXT("KeyFrameTooltip", "Sprite: {0}\nIndex: {1}\nDuration: {2} {3}"),
			SpriteLine,
			FText::AsNumber(FrameIndex),
			FText::AsNumber(KeyFrame->FrameRun),
			FramesText);
	}
	else
	{
		return LOCTEXT("KeyFrameTooltip_Invalid", "Invalid key frame index");
	}
}

FOptionalSize SPixel2DAnimNotifkKeyframeWidget::GetFrameWidth() const
{
	if (const FPaperFlipbookKeyFrame* KeyFrame = GetKeyFrameData())
	{
		return FMath::Max<float>(0, KeyFrame->FrameRun * SlateUnitsPerFrame.Get());
	}
	else
	{
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifyTimelineTrack

void SPixel2DAnimNotifyTimelineTrack::Construct(const FArguments& InArgs)
{
	SlateUnitsPerFrame = InArgs._SlateUnitsPerFrame;
	FlipbookBeingEdited = InArgs._FlipbookBeingEdited;

	ChildSlot
	[
		SAssignNew(MainBoxPtr, SHorizontalBox)
	];

	Rebuild();
}

void SPixel2DAnimNotifyTimelineTrack::Rebuild()
{
	MainBoxPtr->ClearChildren();

	// Create the sections for each keyframe
	if (UPaperFlipbook* Flipbook = FlipbookBeingEdited.Get())
	{
		for (int32 KeyFrameIdx = 0; KeyFrameIdx < Flipbook->GetNumKeyFrames(); ++KeyFrameIdx)
		{
			MainBoxPtr->AddSlot()
			.AutoWidth()
			[
				SNew(SPixel2DAnimNotifkKeyframeWidget, KeyFrameIdx)
				.SlateUnitsPerFrame(this->SlateUnitsPerFrame)
				.FlipbookBeingEdited(this->FlipbookBeingEdited)
			];
		}
	}
}

#undef LOCTEXT_NAMESPACE
