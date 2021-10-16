// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DActorFactory.h"
#include "AssetData.h"
#include "Pixel2DActor.h"
#include "Pixel2DComponent.h"
#include "Pixel2DAnimBlueprint.h"

//////////////////////////////////////////////////////////////////////////
// UPixel2DActorFactory

UPixel2DActorFactory::UPixel2DActorFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = NSLOCTEXT("SpriteAnim", "SpriteFactoryDisplayName", "Add SpriteAnim");
	NewActorClass = APixel2DActor::StaticClass();
}

void UPixel2DActorFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);

	if (UPixel2DAnimBlueprint* SpriteAnim = Cast<UPixel2DAnimBlueprint>(Asset))
	{
		APixel2DActor* TypedActor = CastChecked<APixel2DActor>(NewActor);
		TypedActor->RegisterAnimInstance(SpriteAnim->GeneratedClass);
	}
}

void UPixel2DActorFactory::PostCreateBlueprint(UObject* Asset, AActor* CDO)
{
	if (UPixel2DAnimBlueprint* SpriteAnim = Cast<UPixel2DAnimBlueprint>(Asset))
	{
		if (APixel2DActor* TypedActor = Cast<APixel2DActor>(CDO))
		{
			TypedActor->RegisterAnimInstance(SpriteAnim->GeneratedClass);
		}
	}
}

bool UPixel2DActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	if (AssetData.IsValid() && AssetData.GetClass()->IsChildOf(UPixel2DAnimBlueprint::StaticClass()))
	{
		return true;
	}
	else
	{
		OutErrorMsg = NSLOCTEXT("SpriteAnim", "CanCreateActorFrom_NoSprite", "No sprite was specified.");
		return false;
	}
}
