// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraphSchema_K2.h"
#include "Pixel2DAnimTransitionSchema.generated.h"

class UPixel2DAnimStateNode;
class UPixel2DAnimStateTransitionNode;

// This class is the schema for transition rule graphs in animation state machines
UCLASS(MinimalAPI)
class UPixel2DAnimTransitionSchema : public UEdGraphSchema_K2
{
	GENERATED_UCLASS_BODY()

	//~ Begin UEdGraphSchema Interface.
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual bool CanDuplicateGraph(UEdGraph* InSourceGraph) const override { return false; }
	virtual void GetGraphDisplayInformation(const UEdGraph& Graph, /*out*/ FGraphDisplayInfo& DisplayInfo) const override;
	virtual bool DoesSupportEventDispatcher() const	override { return false; }
	virtual bool ShouldAlwaysPurgeOnModification() const override { return true; }
	virtual void HandleGraphBeingDeleted(UEdGraph& GraphBeingRemoved) const override;
	//~ End UEdGraphSchema Interface.

};
