// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimGraphNode_TransitionResult.h"
#include "GraphEditorSettings.h"


/////////////////////////////////////////////////////
// UPixel2DAnimGraphNode_TransitionResult

#define LOCTEXT_NAMESPACE "UPixel2DAnimGraphNode_TransitionResult"

UPixel2DAnimGraphNode_TransitionResult::UPixel2DAnimGraphNode_TransitionResult(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FLinearColor UPixel2DAnimGraphNode_TransitionResult::GetNodeTitleColor() const
{
	return GetDefault<UGraphEditorSettings>()->ResultNodeTitleColor;
}

FText UPixel2DAnimGraphNode_TransitionResult::GetTooltipText() const
{
	return LOCTEXT("TransitionResultTooltip", "This expression is evaluated to determine if the state transition can be taken");
}

FText UPixel2DAnimGraphNode_TransitionResult::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Result", "Result");
}

void UPixel2DAnimGraphNode_TransitionResult::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// Intentionally empty. This node is auto-generated when a transition graph is created.
}

#undef LOCTEXT_NAMESPACE
