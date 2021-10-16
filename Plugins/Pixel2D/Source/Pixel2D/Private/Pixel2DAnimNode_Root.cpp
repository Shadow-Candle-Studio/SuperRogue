// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNode_Root.h"

/////////////////////////////////////////////////////
// FPixel2DAnimNode_Root

FPixel2DAnimNode_Root::FPixel2DAnimNode_Root()
{
}

void FPixel2DAnimNode_Root::Initialize_AnyThread(const FPixel2DAnimationInitializeContext& Context)
{
	FPixel2DAnimNode_Base::Initialize_AnyThread(Context);
	Result.Initialize(Context);
}

void FPixel2DAnimNode_Root::Update_AnyThread(const FPixel2DAnimationUpdateContext& Context)
{
	EvaluateGraphExposedInputs.Execute(Context);
	Result.Update(Context);
}

void FPixel2DAnimNode_Root::Evaluate_AnyThread(FPixel2DPoseContext& Output)
{
	Result.Evaluate(Output);
}