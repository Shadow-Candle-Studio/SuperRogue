// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNotifyCommands.h"

//////////////////////////////////////////////////////////////////////////
// FPixel2DAnimNotifyCommands

#define LOCTEXT_NAMESPACE "FPixel2DAnimNotifyCommands"

void FPixel2DAnimNotifyCommands::RegisterCommands()
{
	UI_COMMAND(UpdateNotificationPosition, "Move Notification Here", "Updates the position of the notification", EUserInterfaceActionType::Button, FInputChord());

}

#undef LOCTEXT_NAMESPACE