// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimGraphNode_Root.h"
#include "GraphEditorSettings.h"


/////////////////////////////////////////////////////
// FPixel2DPoseLinkMappingRecord

void FPixel2DPoseLinkMappingRecord::PatchLinkIndex(uint8* DestinationPtr, int32 LinkID, int32 SourceLinkID) const
{
	checkSlow(IsValid());

	DestinationPtr = ChildProperty->ContainerPtrToValuePtr<uint8>(DestinationPtr);

	if (ChildPropertyIndex != INDEX_NONE)
	{
		FArrayProperty* ArrayProperty = CastFieldChecked<FArrayProperty>(ChildProperty);

		FScriptArrayHelper ArrayHelper(ArrayProperty, DestinationPtr);
		check(ArrayHelper.IsValidIndex(ChildPropertyIndex));

		DestinationPtr = ArrayHelper.GetRawPtr(ChildPropertyIndex);
	}

	// Check to guard against accidental infinite loops
	check((LinkID == INDEX_NONE) || (LinkID != SourceLinkID));

	// Patch the pose link
	FPixel2DPoseLinkBase& PoseLink = *((FPixel2DPoseLinkBase*)DestinationPtr);
	PoseLink.LinkID = LinkID;
	PoseLink.SourceLinkID = SourceLinkID;
}

/////////////////////////////////////////////////////
// UPixel2DAnimGraphNode_Root

#define LOCTEXT_NAMESPACE "UPixel2DAnimGraphNode_Root"

UPixel2DAnimGraphNode_Root::UPixel2DAnimGraphNode_Root(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FLinearColor UPixel2DAnimGraphNode_Root::GetNodeTitleColor() const
{
	return GetDefault<UGraphEditorSettings>()->ResultNodeTitleColor;
}

FText UPixel2DAnimGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("SpriteAnimGraphNodeRoot_Title", "Final Pixel2D Flipbook");
}

FText UPixel2DAnimGraphNode_Root::GetTooltipText() const
{
	return LOCTEXT("SpriteAnimGraphNodeRoot_Tooltip", "Wire the final sprite animation into this node");
}

bool UPixel2DAnimGraphNode_Root::IsSinkNode() const
{
	return true;
}

void UPixel2DAnimGraphNode_Root::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// Intentionally empty. This node is auto-generated when a new graph is created.
}

#undef LOCTEXT_NAMESPACE
