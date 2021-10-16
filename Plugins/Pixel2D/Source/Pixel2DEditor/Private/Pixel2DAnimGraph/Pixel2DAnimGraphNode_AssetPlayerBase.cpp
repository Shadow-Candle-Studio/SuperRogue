// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimGraphNode_AssetPlayerBase.h"
#include "EdGraphSchema_K2.h"
#include "Pixel2DAnimGraphNode_AssetSprite.h"

void UPixel2DAnimGraphNode_AssetPlayerBase::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object)
	{
		// recache visualization now an asset pin's connection is changed
		if (const UEdGraphSchema* Schema = GetSchema())
		{
			Schema->ForceVisualizationCacheClear();
		}
	}
}

void UPixel2DAnimGraphNode_AssetPlayerBase::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	if (Pin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object)
	{
		// recache visualization now an asset pin's default value has changed
		if (const UEdGraphSchema* Schema = GetSchema())
		{
			Schema->ForceVisualizationCacheClear();
		}
	}
}

UClass* GetNodeClassForAsset(const UClass* AssetClass)
{
	UClass* NodeClass = nullptr;

	// Iterate over all classes..
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass *Class = *ClassIt;
		// Look for AnimGraphNode classes
		if (Class->IsChildOf(UPixel2DAnimGraphNode_Base::StaticClass()))
		{
			// See if this node is the 'primary handler' for this asset type
			const UPixel2DAnimGraphNode_Base* NodeCDO = Class->GetDefaultObject<UPixel2DAnimGraphNode_Base>();
			if (NodeCDO->SupportsAssetClass(AssetClass) == EAnimAssetHandlerType::PrimaryHandler)
			{
				NodeClass = Class;
				break;
			}
		}
	}

	return NodeClass;
}

bool SupportNodeClassForAsset(const UClass* AssetClass, UClass* NodeClass)
{
	// Get node CDO
	const UPixel2DAnimGraphNode_Base* NodeCDO = NodeClass->GetDefaultObject<UPixel2DAnimGraphNode_Base>();
	// See if this node supports this asset type (primary or not)
	return (NodeCDO->SupportsAssetClass(AssetClass) != EAnimAssetHandlerType::NotSupported);
}
