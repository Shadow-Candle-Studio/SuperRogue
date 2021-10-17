// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimBlueprint.h"
#include "Pixel2DAnimBlueprintGeneratedClass.h"

UPixel2DAnimBlueprint::UPixel2DAnimBlueprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseMultiThreadedAnimationUpdate = true;
}

void UPixel2DAnimBlueprint::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

#if WITH_EDITOR

UClass* UPixel2DAnimBlueprint::GetBlueprintClass() const
{
	return UPixel2DAnimBlueprintGeneratedClass::StaticClass();
}

bool UPixel2DAnimBlueprint::CanRecompileWhilePlayingInEditor() const
{
	return true;
}

void UPixel2DAnimBlueprint::PostLoad()
{
	Super::PostLoad();
}

#endif