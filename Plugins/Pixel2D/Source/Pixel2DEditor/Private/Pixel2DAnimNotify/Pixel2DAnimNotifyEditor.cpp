// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNotifyEditor.h"

#include "Runtime/Slate/Public/Framework/Docking/TabManager.h"
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"
#include "Runtime/Slate/Public/Widgets/Layout/SSpacer.h"
#include "SEditorViewport.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBox.h"
#include "Runtime/Slate/Public/Widgets/Views/SListView.h"
#include "SPixel2DAnimNotifyTimeline.h"
#include "Pixel2DAnimNotifyCommands.h"
#include "Pixel2DAnimNotifyViewportClient.h"
#include "PaperFlipbookComponent.h"
#include "SSingleObjectDetailsPanel.h"
#include "Pixel2DAnimNotify.h"

#define LOCTEXT_NAMESPACE "FPixel2DAnimNotifyEditor"

const FName FPixel2DAnimNotifyEditor::Pixel2DAnimNotifyEditorTabName(TEXT("Pixel2DAnimNotifyEditorTabName"));


//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimNotifyViewport

class SPixel2DAnimNotifyViewport : public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SPixel2DAnimNotifyViewport)
		: _FlipbookBeingEdited((UPaperFlipbook*)nullptr)
	{}

	SLATE_ATTRIBUTE(UPaperFlipbook*, FlipbookBeingEdited)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	// End of SEditorViewport interface

	UPaperFlipbookComponent* GetPreviewComponent() const
	{
		return EditorViewportClient->GetPreviewComponent();
	}

private:
	TAttribute<UPaperFlipbook*> FlipbookBeingEdited;

	// Viewport client
	TSharedPtr<FPixel2DAnimNotifyViewportClient> EditorViewportClient;
};

void SPixel2DAnimNotifyViewport::Construct(const FArguments& InArgs)
{
	FlipbookBeingEdited = InArgs._FlipbookBeingEdited;

	SEditorViewport::Construct(SEditorViewport::FArguments());
}

TSharedRef<FEditorViewportClient> SPixel2DAnimNotifyViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(new FPixel2DAnimNotifyViewportClient(FlipbookBeingEdited));

	return EditorViewportClient.ToSharedRef();
}

/////////////////////////////////////////////////////
// SPixel2DNotifyPropertiesTabBody

class SPixel2DNotifyPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(SPixel2DNotifyPropertiesTabBody) {}
	SLATE_END_ARGS()

private:
	// Pointer back to owning notify editor instance (the keeper of state)
	TWeakPtr<class FPixel2DAnimNotifyEditor> NotifyEditorPtr;
public:
	void Construct(const FArguments& InArgs, TSharedPtr<FPixel2DAnimNotifyEditor> InNotifyEditor)
	{
		NotifyEditorPtr = InNotifyEditor;

		SSingleObjectDetailsPanel::Construct(SSingleObjectDetailsPanel::FArguments().HostCommandList(InNotifyEditor->NotifyEditorCommandList).HostTabManager(FGlobalTabmanager::Get()), /*bAutomaticallyObserveViaGetObjectToObserve=*/ true, /*bAllowSearch=*/ true);
	}

	// SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override
	{
		//Memory leak?
		if (!NotifyEditorPtr.IsValid())
		{
			return nullptr;
		}
		
		return NotifyEditorPtr.Pin()->GetEditedNode()->GetNotify();
	}

	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1)
			[
				PropertyEditorWidget
			];
	}
	// End of SSingleObjectDetailsPanel interface
};


//////////////////////////////////////////////////////////////////////////
// FPixel2DAnimNotifyEditor

FPixel2DAnimNotifyEditor::FPixel2DAnimNotifyEditor()
	:NotifyEditorCommandList(MakeShareable(new FUICommandList))
{
	bRegistered = false;
}


TSharedRef<SDockTab> FPixel2DAnimNotifyEditor::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedPtr<FPixel2DAnimNotifyEditor> NotifyEditorPtr = SharedThis(this);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				[
					ViewportPtr.ToSharedRef()
				]
				+SHorizontalBox::Slot()
				.MaxWidth(350)
				[
					SNew(SPixel2DNotifyPropertiesTabBody, NotifyEditorPtr)
				]
			]

			+SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.AutoHeight()
			 [
				 SNew(SPixel2DAnimNotifyTimeline, NotifyEditorCommandList)
				 .EditedNode(this, &FPixel2DAnimNotifyEditor::GetEditedNode)
				 .OnSelectionChanged(this, &FPixel2DAnimNotifyEditor::OnSelectionChanged)
				 .OnMoveNotifyEvent(this, &FPixel2DAnimNotifyEditor::OnMoveNotifyEvent)
			 ]
		];
}

void FPixel2DAnimNotifyEditor::InitNotifyEditor(UPixel2DAnimGraphNode_AssetSprite * InitNode)
{
	EditedNode = InitNode;
	FPixel2DAnimNotifyCommands::Register();

	ViewportPtr = SNew(SPixel2DAnimNotifyViewport)
				  .FlipbookBeingEdited(this, &FPixel2DAnimNotifyEditor::GetEditedFlipbook);

}

void FPixel2DAnimNotifyEditor::OnSelectionChanged(int32 SlotIndex, int32 NewIndex)
{
	// Set speculatively on a right click as it will be used 
	// only when UpdateNotificationPosition() is called
	CurrentNotifySlot = SlotIndex;
	UpdateSelectedFrame(NewIndex);
}

void FPixel2DAnimNotifyEditor::OnMoveNotifyEvent(int32 NotifyIndex)
{
	float NewOffset = EditedNode->Node.UpdateNotifyPosition(NotifyIndex, CurrentNotifySlot);
	if (NotifyIndex < (EditedNode->NotifyProxy->NotifyArray)->Num())
	{
		(EditedNode->NotifyProxy->NotificationEvents[NotifyIndex]).NormalisedTriggerOffset = NewOffset;
	}
}

UPaperFlipbookComponent * FPixel2DAnimNotifyEditor::GetPreviewComponent() const
{
	UPaperFlipbookComponent* PreviewComponent = ViewportPtr->GetPreviewComponent();
	check(PreviewComponent);
	return PreviewComponent;
}

void FPixel2DAnimNotifyEditor::UpdateSelectedFrame(int NewIndex) const
{
	GetPreviewComponent()->SetPlaybackPositionInFrames(NewIndex, false);
};

#undef LOCTEXT_NAMESPACE