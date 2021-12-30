// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FTabManager;

class SKILLEDITOR2D_API LevelSequenceEditorHelpers_SKA
{
public:

	/** Open dialog for creating a master sequence */
	static void OpenMasterSequenceDialog(const TSharedRef<FTabManager>& TabManager);
	
	/** Create a level sequence asset given an asset name and package path */
	static UObject* CreateLevelSequenceAsset(const FString& AssetName, const FString& PackagePath, UObject* AssetToDuplicate = nullptr);
};
