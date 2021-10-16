// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimGraphConnectionDrawingPolicy.h"
#include "Pixel2DAnimBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Pixel2DAnimGraphSchema.h"
#include "Pixel2DAnimGraphNode_Base.h"

/////////////////////////////////////////////////////
// FPixel2DAnimGraphConnectionDrawingPolicy

FPixel2DAnimGraphConnectionDrawingPolicy::FPixel2DAnimGraphConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	: FKismetConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj)
{
}

bool FPixel2DAnimGraphConnectionDrawingPolicy::TreatWireAsExecutionPin(UEdGraphPin* InputPin, UEdGraphPin* OutputPin) const
{
	const UPixel2DAnimGraphSchema* Schema = GetDefault<UPixel2DAnimGraphSchema>();

	return (InputPin != NULL) && (Schema->IsPosePin(OutputPin->PinType));
}

void FPixel2DAnimGraphConnectionDrawingPolicy::DetermineStyleOfExecWire(float& Thickness, FLinearColor& WireColor, bool& bDrawBubbles, const FTimePair& Times)
{
	// It's a followed link, make it strong and yellowish but fading over time
	const double BlendWeight = Times.ThisExecTime;

	const float HeavyBlendThickness = AttackWireThickness;
	const float LightBlendThickness = SustainWireThickness;

	Thickness = FMath::Lerp<float>(LightBlendThickness, HeavyBlendThickness, BlendWeight);
	WireColor = WireColor * (BlendWeight * 0.5f + 0.5f);//FMath::Lerp<FLinearColor>(SustainColor, AttackColor, BlendWeight);

	bDrawBubbles = true;
}

