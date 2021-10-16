// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNode_TransitionResult.h"

/////////////////////////////////////////////////////
// FPixel2DAnimNode_TransitionResult

FPixel2DAnimNode_TransitionResult::FPixel2DAnimNode_TransitionResult() 
	: bCanEnterTransition(false)
{
}

void FPixel2DAnimNode_TransitionResult::Initialize_AnyThread(const FPixel2DAnimationInitializeContext& Context)
{
	FPixel2DAnimNode_Base::Initialize_AnyThread(Context);
}

void FPixel2DAnimNode_TransitionResult::Update_AnyThread(const FPixel2DAnimationUpdateContext& Context)
{
}

void FPixel2DAnimNode_TransitionResult::Evaluate_AnyThread(FPixel2DPoseContext& Output)
{
}
