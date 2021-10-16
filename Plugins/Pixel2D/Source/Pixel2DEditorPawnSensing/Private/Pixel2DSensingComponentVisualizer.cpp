// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DSensingComponentVisualizer.h"
#include "SceneManagement.h"
#include "Pixel2DPawnSensingComponent.h"


void FPixel2DSensingComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (View->Family->EngineShowFlags.VisualizeSenses)
	{
		const UPixel2DPawnSensingComponent* Senses = Cast<const UPixel2DPawnSensingComponent>(Component);
		if (Senses != NULL)
		{
			const FTransform Transform = FTransform(Senses->GetSensorRotation(), Senses->GetSensorLocation());

			// Sight
			if ((Senses->MaxSightDistance > 0.0f) &&
				((Senses->StartAngle != 0.0f) || (Senses->EndAngle != 0.0f)))
			{
				TArray<FVector> Verts;
				DrawArc(PDI, Senses->GetSensorLocation(), FVector::ForwardVector, FVector::UpVector, Senses->StartAngle, Senses->EndAngle, Senses->MaxSightDistance, 10, FColor::Yellow, SDPG_World);

				if (Senses->MinSightDistance > 0.0f)
				{
					DrawArc(PDI, Senses->GetSensorLocation(), FVector::ForwardVector, FVector::UpVector, Senses->StartAngle, Senses->EndAngle, Senses->MinSightDistance, 10, FColor::Red, SDPG_World);
				}
			}
		}
	}
}
