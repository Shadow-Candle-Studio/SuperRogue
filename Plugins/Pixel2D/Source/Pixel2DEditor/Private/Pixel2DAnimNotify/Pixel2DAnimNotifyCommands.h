// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FPixel2DAnimNotifyCommands : public TCommands<FPixel2DAnimNotifyCommands>
{
public:
	FPixel2DAnimNotifyCommands()
		: TCommands<FPixel2DAnimNotifyCommands>(
			TEXT("Pixel2DAnimNotifyEditor"), // Context name for fast lookup
			NSLOCTEXT("Contexts", "Pixel2DEditor", "Pixel2D AnimNotify Editor"), // Localized context name for displaying
			NAME_None, // Parent
			FName(TEXT("Pixel2DStyle")) // Icon Style Set
			)
	{
	}

	// TCommand<> interface
	virtual void RegisterCommands() override;
	// End of TCommand<> interface

public:
	TSharedPtr<FUICommandInfo> UpdateNotificationPosition;

};
