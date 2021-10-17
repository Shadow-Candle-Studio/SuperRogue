// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Pixel2DAnimGraphSchema.h"
#include "Pixel2DAnimStateGraphSchema.generated.h"

UCLASS(MinimalAPI)
class UPixel2DAnimStateGraphSchema : public UPixel2DAnimGraphSchema
{
	GENERATED_UCLASS_BODY()

	//~ Begin UEdGraphSchema Interface.
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetGraphDisplayInformation(const UEdGraph& Graph, /*out*/ FGraphDisplayInfo& DisplayInfo) const override;
	//~ End UEdGraphSchema Interface.
};
