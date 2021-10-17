// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"

class PIXEL2DEDITOR_API FPixel2DAnimBlueprintUtils
{
public:

	/**
	* Removes the supplied graph from the Blueprint.
	*
	* @param Blueprint			The blueprint containing the graph
	* @param GraphToRemove		The graph to remove.
	* @param Flags				Options to control the removal process
	*/
	static void RemoveGraph(UBlueprint* Blueprint, class UEdGraph* GraphToRemove, EGraphRemoveFlags::Type Flags = EGraphRemoveFlags::Default);

};