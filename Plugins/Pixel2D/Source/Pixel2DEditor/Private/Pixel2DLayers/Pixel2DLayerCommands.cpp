// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DLayerCommands.h"

#define LOCTEXT_NAMESPACE "FPixel2DLayerModule"

void FPixel2DLayerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Pixel2DLayers", "Bring up Pixel2D Layers window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
