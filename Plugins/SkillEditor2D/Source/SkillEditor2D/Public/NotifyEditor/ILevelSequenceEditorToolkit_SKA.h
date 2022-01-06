// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AssetEditorToolkit.h"

class ISequencer;

/**
 * Implements an Editor toolkit for level sequences.
 */
class ILevelSequenceEditorToolkit_SKA
	: public FAssetEditorToolkit
{
public:
	
	/**
	 * Access the sequencer that is displayed on this asset editor UI
	 */
	virtual TSharedPtr<ISequencer> GetSequencer() const = 0;
};
