// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "SKANode.h"




/**
 * graph node implementation
 */
class SKILLEDITOR2D_API GraphNodeState: public  SGraphNode
{
public:
	SLATE_BEGIN_ARGS(GraphNodeState){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, USKANode* InNode);
	
	// SNodePanel::SNode interface
	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	// End of SNodePanel::SNode interface

	// SGraphNode interface
	
	
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	// End of SGraphNode interface
	protected:
	FSlateColor GetBorderBackgroundColor() const;
	virtual FText GetPreviewCornerText() const;
	virtual const FSlateBrush* GetNameIcon() const;
};
