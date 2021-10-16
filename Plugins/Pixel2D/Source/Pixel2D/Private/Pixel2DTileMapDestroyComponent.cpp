// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DTileMapDestroyComponent.h"


UPixel2DTileMapDestroyComponent::UPixel2DTileMapDestroyComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPixel2DTileMapDestroyComponent::BeginPlay()
{
	Super::BeginPlay();

	// Promote the tilemap to an instance so that any change becomes
	// a local change
	AActor* Parent = GetOwner();
	if (Parent)
	{
		TArray<UPaperTileMapComponent*> TileMapComps;
		Parent->GetComponents<UPaperTileMapComponent>(TileMapComps);

		if (TileMapComps.Num() != 0)
		{
			TileMapComps[0]->MakeTileMapEditable();
		}
	}
}

void UPixel2DTileMapDestroyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	RestoreCollision();
}

void UPixel2DTileMapDestroyComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
	RestoreCollision();
}

void UPixel2DTileMapDestroyComponent::RestoreCollision()
{
	if (HiddenCells.Num() == 0)
	{
		return;
	}

	AActor * Parent = GetOwner();
	if (Parent)
	{
		TArray<UPaperTileMapComponent*> TileMapComps;
		Parent->GetComponents<UPaperTileMapComponent>(TileMapComps);

		if (TileMapComps.Num() != 0)
		{
			RestoreHiddenTile(TileMapComps[0]->TileMap);
			if (TileMapComps[0]->TileMap)
			{
				TileMapComps[0]->TileMap->RebuildCollision();
			}
			TileMapComps[0]->RebuildCollision();
		}
	}
}