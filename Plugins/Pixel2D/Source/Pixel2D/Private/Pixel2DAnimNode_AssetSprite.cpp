// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNode_AssetSprite.h"
#include "Pixel2DAnimNotifyQueue.h"

void FPixel2DAnimNode_AssetSprite::Initialize_AnyThread(const FPixel2DAnimationInitializeContext & Context)
{
	ElapsedTime = 0;
	RelativeTimeInFlipbook = 0;
	for (int i = 0; i < NotifyEvents.Num(); i++)
	{
		NotifyEvents[i].bTriggered = false;
	}
}

void FPixel2DAnimNode_AssetSprite::Update_AnyThread(const FPixel2DAnimationUpdateContext & Context)
{
	ElapsedTime += Context.DeltaTime;
	RelativeTimeInFlipbook += Context.DeltaTime;

	if (AssetFlipbook == nullptr)
		return;

	float TotalTime = AssetFlipbook->GetTotalDuration();

	for (int i = 0; i < NotifyEvents.Num(); i++)
	{
		if (NotifyEvents[i].CanTrigger(RelativeTimeInFlipbook, TotalTime))
		{
			(Context.NotifyQueue)->AddAnimNotify(&NotifyEvents[i], AssetFlipbook);
			NotifyEvents[i].bTriggered = true;
		}
	}
}

void FPixel2DAnimNode_AssetSprite::UpdateAssetPlayer(const FPixel2DAnimationUpdateContext & Context)
{
}

void FPixel2DAnimNode_AssetSprite::Evaluate_AnyThread(FPixel2DPoseContext & Output)
{
	Output.Flipbook = AssetFlipbook;

	if (RelativeTimeInFlipbook >= GetCurrentAssetLength())
	{
		RelativeTimeInFlipbook -= GetCurrentAssetLength();
		for (int i = 0; i < NotifyEvents.Num(); i++)
		{
			if (NotifyEvents[i].bEnabled && NotifyEvents[i].bTriggered)
			{
				NotifyEvents[i].bTriggered = false;
			}
		}
	}
}

float FPixel2DAnimNode_AssetSprite::GetCurrentAssetLength()
{
	return AssetFlipbook ? AssetFlipbook->GetTotalDuration() : 0;
}

float FPixel2DAnimNode_AssetSprite::UpdateNotifyPosition(int32 NotifyIndex, int32 SlotIndex)
{
	if (AssetFlipbook == nullptr)
	{
		return 0;
	}

	if (NotifyIndex > NotifyEvents.Num())
	{
		return 0;
	}

	int32 MaxSlots = MaxNotifySlots * AssetFlipbook->GetNumFrames();
	float NewOffset = FMath::Clamp<float>(((float)SlotIndex) / MaxSlots, 0.f, 1.f);
	NotifyEvents[NotifyIndex].NormalisedTriggerOffset = NewOffset;

	return NewOffset;
}

bool FPixel2DAnimNode_AssetSprite::IsNotifyInRange(int32 SlotIndex)
{
	if (AssetFlipbook == nullptr)
		return false;

	int32 MaxSlots = MaxNotifySlots * AssetFlipbook->GetNumFrames();
	for (int i = 0; i < NotifyEvents.Num(); i++)
	{
		if ((int32)(MaxSlots * NotifyEvents[i].NormalisedTriggerOffset) == SlotIndex)
		{
			return true;
		}
	}
	return false;
}
