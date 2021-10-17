// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "IDocumentation.h"

#define LOCTEXT_NAMESPACE "Pixel2DAssetBrowser"

struct FPixel2DAnimAssetBrowserSummoner : public FWorkflowTabFactory
{
	FPixel2DAnimAssetBrowserSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp, bool bInShowHistory);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;

	// Create a tooltip widget for the tab
	virtual TSharedPtr<SToolTip> CreateTabToolTipWidget(const FWorkflowTabSpawnInfo& Info) const override
	{
		return  IDocumentation::Get()->CreateToolTip(LOCTEXT("AnimAssetBrowserTooltip", "The Asset Browser lets you browse all flipbook assets."), NULL, TEXT(""), TEXT(""));
	}

private:
	bool bShowHistory;
};

#undef LOCTEXT_NAMESPACE