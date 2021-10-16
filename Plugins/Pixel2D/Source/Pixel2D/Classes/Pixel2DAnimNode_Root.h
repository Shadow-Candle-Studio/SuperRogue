// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Pixel2DAnimNode_Base.h"
#include "Pixel2DAnimNode_Root.generated.h"

// Root node of an animation tree (sink)
USTRUCT(BlueprintInternalUseOnly)
struct PIXEL2D_API FPixel2DAnimNode_Root : public FPixel2DAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
	FPixel2DPoseLink Result;

public:
	FPixel2DAnimNode_Root();

	// FPixel2DAnimNode_Base interface
	virtual void Initialize_AnyThread(const FPixel2DAnimationInitializeContext& Context) override;
	virtual void Update_AnyThread(const FPixel2DAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPixel2DPoseContext& Output) override;
	// End of FPixel2DAnimNode_Base interface
};
