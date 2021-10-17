// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "AssetData.h"
#include "Widgets/SToolTip.h"
#include "PersonaDelegates.h"
#include "Editor/ContentBrowser/Public/ContentBrowserDelegates.h"
#include "EditorAnimUtils.h"
#include "IAnimationSequenceBrowser.h"
#include "ARFilter.h"

class FUICommandList;
class SMenuAnchor;
class UAnimationAsset;
class FFrontendFilter_Folder;

//////////////////////////////////////////////////////////////////////////
// SPixel2DAnimAssetBrowser

class SPixel2DAnimAssetBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPixel2DAnimAssetBrowser)
		: _ShowHistory(false)
	{}

	SLATE_ARGUMENT(bool, ShowHistory)

	SLATE_ARGUMENT(FOnOpenNewAsset, OnOpenNewAsset)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

	void OnRequestOpenAsset(const FAssetData& AssetData, bool bFromHistory);

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual ~SPixel2DAnimAssetBrowser();

	/** IAnimationSequenceBrowser interface */
	virtual void SelectAsset(UAnimationAsset * AnimAsset);

	/** Delegate that handles creation of context menu */
	TSharedPtr<SWidget> OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets);

	/** Delegate to handle "Find in Content Browser" context menu option */
	void FindInContentBrowser();

	/** Delegate to handle enabling the "Find in Content Browser" context menu option */
	bool CanFindInContentBrowser() const;

	/** Delegate to handle "Save" context menu option */
	void SaveSelectedAssets(TArray<FAssetData> ObjectsToSave) const;

	/** Delegate to handle enabling the "Save" context menu option */
	bool CanSaveSelectedAssets(TArray<FAssetData> ObjectsToSave) const;

	/** public reference to add to history */
	virtual void AddToHistory(UAnimationAsset * AnimAsset);

protected:
	bool CanShowColumnForAssetRegistryTag(FName AssetType, FName TagName) const;

	/** Populate supplied OutPackages with the packages for the supplied Assets array */
	void GetSelectedPackages(const TArray<FAssetData>& Assets, TArray<UPackage*>& OutPackages) const;

	/** Adds the supplied asset to the asset history */
	void AddAssetToHistory(const FAssetData& AssetData);

	/** Single step forward in history */
	FReply OnGoForwardInHistory();

	/** Single step back in history */
	FReply OnGoBackInHistory();

	/** Jumps immediately to an index in the history if valid */
	void GoToHistoryIndex(int32 InHistoryIdx);

	/** Returns TRUE if stepping backward in history is allowed */
	bool CanStepBackwardInHistory() const;

	/** Returns TRUE if stepping forward in history is allowed */
	bool CanStepForwardInHistory() const;

	/**
	* Mouse down callback to display a history menu
	*
	* @param InMenuAnchor		This is the anchor the menu will use for positioning
	*/
	FReply OnMouseDownHistory(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TWeakPtr< SMenuAnchor > InMenuAnchor);

	/**
	* Callback to create the history menu.
	*
	* @param bInBackHistory		TRUE if the back history is requested, FALSE if the forward history is
	*
	* @return						The menu widget displaying all available history
	*/
	TSharedRef<SWidget> CreateHistoryMenu(bool bInBackHistory) const;

	/**
	* Create a custom asset view tooltip for the provided asset
	*/
	TSharedRef<SToolTip> CreateCustomAssetToolTip(FAssetData& AssetData);

	/**
	* Called as a tooltip is about to show;
	*/
	bool OnVisualizeAssetToolTip(const TSharedPtr<SWidget>& TooltipContent, FAssetData& AssetData);

	/**
	* Called as a tooltip is closing
	*/
	void OnAssetToolTipClosing();

	/**
	* Has the animation asset preview tool tip been visualized
	*/
	bool IsToolTipPreviewVisible();

	/** Returns visible when not in a Blueprint mode (anim mode, etc...) */
	EVisibility GetHistoryVisibility() const;

	/** Perform additional filtering */
	bool HandleFilterAsset(const FAssetData& InAssetData) const;


protected:

	/**
	* Commands handled by this widget
	*/
	TSharedPtr<FUICommandList> Commands;

	// Set of tags to prevent creating details view columns for (infrequently used)
	TSet<FName> AssetRegistryTagsToIgnore;

	// List of recently opened assets
	TArray<FAssetData> AssetHistory;

	// Current position in the asset history
	int32 CurrentAssetHistoryIndex;

	// Max assets to save in history
	static const int32 MaxAssetsHistory;

	// Track if we have tried to cache the first asset we were playing
	bool bTriedToCacheOrginalAsset;

	/** Whether to show the history widgets */
	bool bShowHistory;

	/** Delegate called to open a new asset for editing */
	FOnOpenNewAsset OnOpenNewAsset;

	// delegate to sync the asset picker to selected assets
	FSyncToAssetsDelegate SyncToAssetsDelegate;
	FGetCurrentSelectionDelegate GetCurrentSelectionDelegate;

	/** Delegate used to set the AR filter after the fact */
	FSetARFilterDelegate SetFilterDelegate;

	/** Keep the AR filter around so we can modify it */
	FARFilter Filter;

	/** All the folder filters we have */
	TArray<TSharedPtr<FFrontendFilter_Folder>> FolderFilters;

private:

	/** Whether the tooltip was visualized this frame */
	uint8 bToolTipClosedThisFrame : 1;

	/** Whether the active timer is currently registered */
	uint8 bIsActiveTimerRegistered : 1;

	/** Whether the active timer should stop */
	uint8 bToolTipVisualizedThisFrame : 1;

public:
	/** The section of EditorPerProjectUserSettings in which to save settings */
	static const FString SettingsIniSection;
};
