// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Input/DragAndDrop.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SPixel2DAnimNotifyTimeline.h"
#include "Types/SlateStructs.h"
#include "PaperFlipbook.h"
#include "Pixel2DAnimGraphNode_AssetSprite.h"
#include "ScopedTransaction.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"

class FUICommandList;
class SHorizontalBox;

class SPixel2DAnimNotifyTimelineNotify : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SPixel2DAnimNotifyTimelineNotify)
		: _Active(false)
	{}
		SLATE_ATTRIBUTE(bool, Active)
		SLATE_ATTRIBUTE(TArray<FName>, EventNames)
		SLATE_EVENT(FOnPixel2DAnimNotifySelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FOnPixel2DAnimNotifyNotifyEventMoved, OnMoveNotifyEvent)
	SLATE_END_ARGS()

	void Construct(const FArguments & InArgs, int32 InSlotIndex, int32 InFrameIndex, TSharedPtr<FUICommandList> InCommandList);

	// SWidget interface
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	// End of SWidget interface

private:
	TSharedRef<SWidget> GenerateContextMenu();
	void MoveNotify(int NotifyIndex);

private:
	int32 SlotIndex;
	int32 FrameIndex;
	TAttribute<bool> Active;
	TAttribute<TArray<FName>> EventNames;
	TSharedPtr<FUICommandList> CommandList;
	FOnPixel2DAnimNotifySelectionChanged OnSelectionChanged;
	FOnPixel2DAnimNotifyNotifyEventMoved OnMoveNotifyEvent;
};

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifyTimelineSlots

class SPixel2DAnimNotifyTimelineSlots : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SPixel2DAnimNotifyTimelineSlots)
		: _EditedNode(nullptr)
	{}

		SLATE_ATTRIBUTE(class UPixel2DAnimGraphNode_AssetSprite*, EditedNode)
		SLATE_EVENT(FOnPixel2DAnimNotifySelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FOnPixel2DAnimNotifyNotifyEventMoved, OnMoveNotifyEvent)
	SLATE_END_ARGS()

	void Construct(const FArguments & InArgs, TSharedPtr<FUICommandList> InCommandList);

	void Rebuild();

private:
	TAttribute<class UPixel2DAnimGraphNode_AssetSprite *> EditedNode;
	TSharedPtr<FUICommandList> CommandList;
	FOnPixel2DAnimNotifySelectionChanged OnSelectionChanged;
	FOnPixel2DAnimNotifyNotifyEventMoved OnMoveNotifyEvent;

	TSharedPtr<SHorizontalBox> MainBoxPtr;

	// SWidget interface
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		return FReply::Unhandled();
	}
	// End of SWidget interface

	bool IsNotifyActive(int32 SlotIdx) const;
	TArray<FName> GetEventNames() const;

};