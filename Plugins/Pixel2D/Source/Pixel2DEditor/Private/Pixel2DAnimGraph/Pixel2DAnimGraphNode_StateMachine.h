// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Pixel2DAnimNode_StateMachine.h"
#include "Pixel2DAnimGraphNode_StateMachineBase.h"
#include "Pixel2DAnimGraphNode_StateMachine.generated.h"

UCLASS(MinimalAPI)
class UPixel2DAnimGraphNode_StateMachine : public UPixel2DAnimGraphNode_StateMachineBase
{
	GENERATED_UCLASS_BODY()

	// Runtime state machine representation (empty; populated during compilation)
	UPROPERTY(EditAnywhere, Category = Settings)
	FPixel2DAnimNode_StateMachine Node;

	// UPixel2DAnimGraphNode_StateMachineBase interface
	virtual FPixel2DAnimNode_StateMachine& GetNode() override { return Node; }
	// End of UAnimGraphNode_StateMachineBase interface
};
