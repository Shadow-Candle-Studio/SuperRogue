// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SPixel2DAnimNotifyTimelineSlots.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Layout/WidgetPath.h"
#include "Textures/SlateIcon.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/Commands/GenericCommands.h"
#include "Pixel2DAnimNotifyCommands.h"

#define LOCTEXT_NAMESPACE "AnimNotifyEditor"

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifyTimelineNotify

void SPixel2DAnimNotifyTimelineNotify::Construct(const FArguments & InArgs, int32 InSlotIndex, int32 InFrameIndex, TSharedPtr<FUICommandList> InCommandList)
{
	Active                 = InArgs._Active;
	EventNames			   = InArgs._EventNames;
	SlotIndex              = InSlotIndex;
	FrameIndex             = InFrameIndex;
	OnSelectionChanged     = InArgs._OnSelectionChanged;
	CommandList            = InCommandList;
	OnMoveNotifyEvent      = InArgs._OnMoveNotifyEvent;

	ChildSlot
	[
			SNew(SBox)
			.HeightOverride(40)
	];

	if (Active.Get())
	{
		BorderBackgroundColor.Set(FSlateColor(FLinearColor::Red));
	}
	else
	{
		BorderBackgroundColor.Set(FSlateColor(FLinearColor::Transparent));
	}
}

void SPixel2DAnimNotifyTimelineNotify::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{

	if (Active.Get())
	{
		BorderBackgroundColor.Set(FSlateColor(FLinearColor::Red));

	}
	else
	{
		BorderBackgroundColor.Set(FSlateColor(FLinearColor::Transparent));
	}
}

TSharedRef<SWidget> SPixel2DAnimNotifyTimelineNotify::GenerateContextMenu()
{
	const FPixel2DAnimNotifyCommands& Commands = FPixel2DAnimNotifyCommands::Get();
	OnSelectionChanged.ExecuteIfBound(SlotIndex, FrameIndex);

	FMenuBuilder MenuBuilder(true, NULL);
	{
		FNumberFormattingOptions NoCommas;
		NoCommas.UseGrouping = false;

		const FText KeyframeSectionTitle = FText(LOCTEXT("KeyframeXActionsSectionHeader", "Actions"));
		MenuBuilder.BeginSection("KeyframeActions", KeyframeSectionTitle);

		TArray<FName> MenuEntryNames = EventNames.Get();
		if (MenuEntryNames.Num() == 0)
		{
			auto NewFolderAction = FExecuteAction::CreateSP(this, &SPixel2DAnimNotifyTimelineNotify::MoveNotify, -1);
			MenuBuilder.AddMenuEntry(
				LOCTEXT("MoveNotifyMenuText", "No notifies added."),
				FText(),
				FSlateIcon(),
				FUIAction(NewFolderAction));// ,
		}
		else
		{
			for (int i = 0; i < MenuEntryNames.Num(); i++)
			{
				auto NewFolderAction = FExecuteAction::CreateSP(this, &SPixel2DAnimNotifyTimelineNotify::MoveNotify, i);
				MenuBuilder.AddMenuEntry(
					FText::Format(LOCTEXT("MoveNotifyMenuText", "Move {0} here."), FText::FromName(MenuEntryNames[i])),
					FText(),
					FSlateIcon(),
					FUIAction(NewFolderAction));
			}
		}
		MenuBuilder.EndSection();
	}

	return MenuBuilder.MakeWidget();
}

void SPixel2DAnimNotifyTimelineNotify::MoveNotify(int NotifyIndex)
{
	if (NotifyIndex == -1)
	{
		return;
	}
	OnMoveNotifyEvent.ExecuteIfBound(NotifyIndex);
	return;
}

FReply SPixel2DAnimNotifyTimelineNotify::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnSelectionChanged.ExecuteIfBound(SlotIndex, FrameIndex);

	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		TSharedRef<SWidget> MenuContents = GenerateContextMenu();
		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContents, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifyTimelineSlots

void SPixel2DAnimNotifyTimelineSlots::Construct(const FArguments& InArgs, TSharedPtr<FUICommandList> InCommandList)
{
	CommandList            = InCommandList;
	EditedNode             = InArgs._EditedNode;
	OnSelectionChanged     = InArgs._OnSelectionChanged;
	OnMoveNotifyEvent      = InArgs._OnMoveNotifyEvent;


	ChildSlot
	[
		SAssignNew(MainBoxPtr, SHorizontalBox)
	];

	Rebuild();
}

void SPixel2DAnimNotifyTimelineSlots::Rebuild()
{
	MainBoxPtr->ClearChildren();

	if (UPixel2DAnimGraphNode_AssetSprite * Node = EditedNode.Get())
	{
		if (Node->GetAnimationAsset())
		{
			int32 MaxSlots = FPixel2DAnimNode_AssetSprite::MaxNotifySlots * Node->GetAnimationAsset()->GetNumFrames();
			for (int32 SlotIdx = 0; SlotIdx < MaxSlots; ++SlotIdx)
			{
				// Need to use a +1 because FlipbookComponent's SetPlaybackTime treats
				// the time intervals as closed intervals.
				int32 FrameIdx = SlotIdx/FPixel2DAnimNode_AssetSprite::MaxNotifySlots+1;
				MainBoxPtr->AddSlot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					SNew(SPixel2DAnimNotifyTimelineNotify, SlotIdx, FrameIdx, CommandList)
					.OnSelectionChanged(OnSelectionChanged)
					.OnMoveNotifyEvent(OnMoveNotifyEvent)
					.Active(this, &SPixel2DAnimNotifyTimelineSlots::IsNotifyActive, SlotIdx)
					.EventNames(this, &SPixel2DAnimNotifyTimelineSlots::GetEventNames)
					.RenderOpacity(1)
				];
			}
		}
	}
}

bool SPixel2DAnimNotifyTimelineSlots::IsNotifyActive(int32 SlotIdx) const
{
	return (EditedNode.Get()->Node.IsNotifyInRange(SlotIdx));
}

TArray<FName> SPixel2DAnimNotifyTimelineSlots::GetEventNames() const
{
	return (EditedNode.Get())->NotifyEventsNames();
}

#undef LOCTEXT_NAMESPACE
