// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SPixel2DGraphNodeConduit.h"
#include "Pixel2DAnimStateNodeBase.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SToolTip.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "IDocumentation.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

/////////////////////////////////////////////////////
// SStateMachineOutputPin

class SStateMachineOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SStateMachineOutputPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	// End SGraphPin interface

	const FSlateBrush* GetPinBorder() const;
};

void SStateMachineOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	typedef SStateMachineOutputPin ThisClass;

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SStateMachineOutputPin::GetPinBorder)
		.BorderBackgroundColor(this, &ThisClass::GetPinColor)
		.OnMouseButtonDown(this, &ThisClass::OnPinMouseDown)
		.Cursor(this, &ThisClass::GetPinCursor)
	);
}

TSharedRef<SWidget>	SStateMachineOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SStateMachineOutputPin::GetPinBorder() const
{
	return (IsHovered())
		? FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.BackgroundHovered"))
		: FEditorStyle::GetBrush(TEXT("Graph.StateNode.Pin.Background"));
}

/////////////////////////////////////////////////////
// SPixel2DGraphNodeState

void SPixel2DGraphNodeState::Construct(const FArguments& InArgs, UPixel2DAnimStateNodeBase* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}


FSlateColor SPixel2DGraphNodeState::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);

	return InactiveStateColor;
}

void SPixel2DGraphNodeState::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0)
			.BorderBackgroundColor(this, &SPixel2DGraphNodeState::GetBorderBackgroundColor)
			[
				SNew(SOverlay)

				// PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// STATE NAME AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.ColorSpill"))
					.BorderBackgroundColor(TitleShadowColor)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							// POPUP ERROR MESSAGE
							SAssignNew(ErrorText, SErrorText)
							.BackgroundColor(this, &SPixel2DGraphNodeState::GetErrorColor)
							.ToolTipText(this, &SPixel2DGraphNodeState::GetErrorMsgToolTip)
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage)
							.Image(NodeTypeIcon)
						]
						+SHorizontalBox::Slot()
						.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SAssignNew(InlineEditableText, SInlineEditableTextBlock)
								.Style(FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
								.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
								.OnVerifyTextChanged(this, &SPixel2DGraphNodeState::OnVerifyNameTextChanged)
								.OnTextCommitted(this, &SPixel2DGraphNodeState::OnNameTextCommited)
								.IsReadOnly(this, &SPixel2DGraphNodeState::IsNameReadOnly)
								.IsSelected(this, &SPixel2DGraphNodeState::IsSelectedExclusively)
							]
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								NodeTitle.ToSharedRef()
							]
						]
					]
				]
			]
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SPixel2DGraphNodeState::CreatePinWidgets()
{
	UPixel2DAnimStateNodeBase* StateNode = CastChecked<UPixel2DAnimStateNodeBase>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SStateMachineOutputPin, CurPin);

		this->AddPin(NewPin.ToSharedRef());
	}
}

void SPixel2DGraphNodeState::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

TSharedPtr<SToolTip> SPixel2DGraphNodeState::GetComplexTooltip()
{
	UPixel2DAnimStateNodeBase* StateNode = CastChecked<UPixel2DAnimStateNodeBase>(GraphNode);

	return SNew(SToolTip)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
			[
				IDocumentation::Get()->CreateToolTip(FText::FromString("Documentation"), NULL, StateNode->GetDocumentationLink(), StateNode->GetDocumentationExcerptName())
			]
		];
}

FText SPixel2DGraphNodeState::GetPreviewCornerText() const
{
	UPixel2DAnimStateNodeBase* StateNode = CastChecked<UPixel2DAnimStateNodeBase>(GraphNode);

	return FText::Format(NSLOCTEXT("SPixel2DGraphNodeAnimState", "PreviewCornerStateText", "{0} state"), FText::FromString(StateNode->GetStateName()));
}

const FSlateBrush* SPixel2DGraphNodeState::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

/////////////////////////////////////////////////////
// SGraphNodeAnimConduit

void SPixel2DGraphNodeConduit::Construct(const FArguments& InArgs, UAnimStateConduitNode* InNode)
{
}

FText SPixel2DGraphNodeConduit::GetPreviewCornerText() const
{
	UPixel2DAnimStateNodeBase* StateNode = CastChecked<UPixel2DAnimStateNodeBase>(GraphNode);

	return FText::Format(NSLOCTEXT("SGraphNodeAnimState", "PreviewCornerConduitText", "{0} conduit"), FText::FromString(StateNode->GetStateName()));
}

const FSlateBrush* SPixel2DGraphNodeConduit::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.ConduitNode.Icon"));
}
