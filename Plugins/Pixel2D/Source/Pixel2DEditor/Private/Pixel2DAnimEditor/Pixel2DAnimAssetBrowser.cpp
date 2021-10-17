// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimAssetBrowser.h"
#include "Runtime/SlateCore/Public/Widgets/SNullWidget.h"
#include "Pixel2DAnimBlueprintEditor.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "SPixel2DAnimAssetBrowser.h"


#define LOCTEXT_NAMESPACE "AssetBrowser"

FPixel2DAnimAssetBrowserSummoner::FPixel2DAnimAssetBrowserSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp, bool bInShowHistory)
	: FWorkflowTabFactory(Pixel2DAnimationBlueprintEditorTabs::AssetBrowserTab, InHostingApp)
	, bShowHistory(bInShowHistory)
{
	TabLabel = LOCTEXT("AssetBrowserTabTitle", "Asset Browser");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.TabIcon");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("AssetBrowser", "Asset Browser");
	ViewMenuTooltip = LOCTEXT("AssetBrowser_ToolTip", "Shows the flipbook asset browser");
}

TSharedRef<SWidget> FPixel2DAnimAssetBrowserSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedRef<SPixel2DAnimAssetBrowser> Widget = SNew(SPixel2DAnimAssetBrowser)
		.OnOpenNewAsset(nullptr)
		.ShowHistory(bShowHistory);

	return Widget;
}

#undef LOCTEXT_NAMESPACE