// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "KismetNodes/SGraphNodeK2Base.h"

class UPixel2DAnimGraphNode_Base;

class SPixel2DGraphNode : public SGraphNodeK2Base
{
public:
	SLATE_BEGIN_ARGS(SPixel2DGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UPixel2DAnimGraphNode_Base* InNode);

protected:
	// SGraphNode interface
	virtual TArray<FOverlayWidgetInfo> GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const override;
	// End of SGraphNode interface

private:
	/** Keep a reference to the indicator widget handing around */
	TSharedPtr<SWidget> IndicatorWidget;
};
