// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNotifyEvent.h"

bool FPixel2DAnimNotifyEvent::CanTrigger(float ElapsedTime, float TotalTime)
{
	if ((!bEnabled) || (bTriggered))
		return false;

	return (ElapsedTime > TotalTime*NormalisedTriggerOffset);
}
