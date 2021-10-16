// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "Pixel2DAnimNode_Base.h"
#include "Pixel2DAnimNotifyEvent.h"
#include "Pixel2DAnimNode_AssetSprite.generated.h"

class UPaperFlipbook;

USTRUCT()
struct PIXEL2D_API FPixel2DAnimNode_AssetSprite : public FPixel2DAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	const static int32 MaxNotifySlots = 20;

public:
	UPROPERTY()
	UPaperFlipbook * AssetFlipbook;

	UPROPERTY()
	TArray<FPixel2DAnimNotifyEvent> NotifyEvents;

	float ElapsedTime;
	float RelativeTimeInFlipbook = 0;

	/** Get the animation asset associated with the node, derived classes should implement this */
	virtual UPaperFlipbook* GetAnimAsset() { return AssetFlipbook; }

	// FPixel2DAnimNode_Base interface
	virtual void Initialize_AnyThread(const FPixel2DAnimationInitializeContext& Context) override;
	virtual void Update_AnyThread(const FPixel2DAnimationUpdateContext& Context);
	virtual void UpdateAssetPlayer(const FPixel2DAnimationUpdateContext& Context);
	virtual void Evaluate_AnyThread(FPixel2DPoseContext& Output) override;
	// End of FPixel2DAnimNode_Base interface

	// Functions to report data to getters
	virtual float GetCurrentAssetLength();
	virtual FName GetNotifyName(int32 NotifyIndex) {
		return NotifyEvents[NotifyIndex].NotifyName;
	}

	virtual float UpdateNotifyPosition(int32 NotifyIndex, int32 SlotIndex);
	virtual bool IsNotifyInRange(int32 SlotIndex);
};