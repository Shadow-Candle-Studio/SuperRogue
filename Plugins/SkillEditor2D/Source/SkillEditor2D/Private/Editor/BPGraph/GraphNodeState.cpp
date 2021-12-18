// Fill out your copyright notice in the Description page of Project Settings.


#include "GraphNodeState.h"

#include "SInlineEditableTextBlock.h"
#include "SKAOutputPin.h"



//GraphNodeState



void GraphNodeState::Construct(const FArguments& InArgs, USKANode* InNode)
{
	this->GraphNode = InNode;
	this->SetCursor(EMouseCursor::CardinalCross);
	this->UpdateGraphNode();
}

void GraphNodeState::GetNodeInfoPopups(FNodeInfoContext* Context,
	TArray<FGraphInformationPopupInfo>& GraphInformationPopupInfos) const
{
	if (USKANode* StateNode = Cast<USKANode>(GraphNode))
	{
		FLinearColor CurrentStateColor(1.f, 0.5f, 0.25f);
		GraphInformationPopupInfos.Emplace(nullptr, CurrentStateColor, StateNode->GetStateName());
	}
}

void GraphNodeState::UpdateGraphNode()
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

	this->ContentScale.Bind( this, &SGraphNode::GetContentScale );
	this->GetOrAddSlot( ENodeZone::Center )
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage( FEditorStyle::GetBrush( "Graph.StateNode.Body" ) )
			.Padding(0)
			.BorderBackgroundColor( this, &GraphNodeState::GetBorderBackgroundColor )
			[
				SNew(SOverlay)

				// PIN AREA
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// STATE NAME AREA
				+SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SNew(SBorder)
					.BorderImage( FEditorStyle::GetBrush("Graph.StateNode.ColorSpill") )
					.BorderBackgroundColor( TitleShadowColor )
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						[
							// POPUP ERROR MESSAGE
							SAssignNew(ErrorText, SErrorText )
							.BackgroundColor( this, &GraphNodeState::GetErrorColor )
							.ToolTipText( this, &GraphNodeState::GetErrorMsgToolTip )
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
								.Style( FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText" )
								.Text( NodeTitle.Get(), &SNodeTitle::GetHeadTitle )
								.OnVerifyTextChanged(this, &GraphNodeState::OnVerifyNameTextChanged)
								.OnTextCommitted(this, &GraphNodeState::OnNameTextCommited)
								.IsReadOnly( this, &GraphNodeState::IsNameReadOnly )
								.IsSelected(this, &GraphNodeState::IsSelectedExclusively)
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

void GraphNodeState::CreatePinWidgets()
{
	USKANode* StateNode = CastChecked<USKANode>(GraphNode);

	UEdGraphPin* CurPin = StateNode->GetOutputPin();
	if (!CurPin->bHidden)
	{
		TSharedPtr<SGraphPin> NewPin = SNew(SKAOutputPin, CurPin);

		AddPin(NewPin.ToSharedRef());
	}
}

void GraphNodeState::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner( SharedThis(this) );
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);

}

TSharedPtr<SToolTip> GraphNodeState::GetComplexTooltip()
{
	return SNew(SToolTip)
		[
			SNew(SVerticalBox)	
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0.0f, 5.0f, 0.0f, 0.0f))
		];
}

FSlateColor GraphNodeState::GetBorderBackgroundColor() const
{
	FLinearColor InactiveStateColor(0.08f, 0.08f, 0.08f);
	FLinearColor ActiveStateColorDim(0.4f, 0.3f, 0.15f);
	FLinearColor ActiveStateColorBright(1.f, 0.6f, 0.35f);
	return InactiveStateColor;
}

FText GraphNodeState::GetPreviewCornerText() const
{
	USKANode* StateNode = CastChecked<USKANode>(GraphNode);
	return FText::Format(NSLOCTEXT("SGraphNodeState", "PreviewCornerStateText", "{0} state"), FText::FromString(StateNode->GetStateName()));
}

const FSlateBrush* GraphNodeState::GetNameIcon() const
{
	return FEditorStyle::GetBrush( TEXT("Graph.StateNode.Icon") );
}
