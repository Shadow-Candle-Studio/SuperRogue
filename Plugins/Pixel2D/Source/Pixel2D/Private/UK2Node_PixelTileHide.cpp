// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UK2Node_PixelTileHide.h"


#include "Paper2D/Classes/PaperTileSet.h"
#include "Paper2D/Classes/PaperTileMap.h"
#include "Paper2D/Classes/PaperTileLayer.h"
#include "Paper2D/Classes/PaperTileMapActor.h"
#include "Paper2D/Classes/PaperTileMapComponent.h"
#include "Paper2D/Classes/PaperFlipbook.h"
#include "Paper2D/Classes/PaperFlipbookComponent.h"
#include "Paper2D/Classes/PaperFlipbookActor.h"

#include "Pixel2DTileMapDestroyComponent.h"
#include "PhysicsEngine/BodySetup.h"

#define LOCTEXT_NAMESPACE "PixelTileHide"


bool UK2Node_PixelTileHide::CheckData(const APaperTileMapActor * InTileMapActor, TArray<UPixel2DTileMapDestroyComponent*> & OutStaticComps)
{
	if (!InTileMapActor)
	{
		return false;
	}

	// Find the tile destroy component
	InTileMapActor->GetComponents<UPixel2DTileMapDestroyComponent>(OutStaticComps);
	if (OutStaticComps.Num() == 0)
	{
		return false;
	}

	UActorComponent * RenderComponent = InTileMapActor->GetRenderComponent();
	if (!RenderComponent)
	{
		return false;
	}

	UPaperTileMapComponent * TileMapComponent = Cast<UPaperTileMapComponent>(RenderComponent);
	if (!TileMapComponent)
	{
		return false;
	}

	UPaperTileMap * TileMap = TileMapComponent->TileMap;
	if (!TileMap)
	{
		return false;
	}

	return true;
}

void UK2Node_PixelTileHide::HideActorTile(APaperTileMapActor * TileMapActor, int Layer, int PosX, int PosY, bool RebuildCollision)
{
	TArray<UPixel2DTileMapDestroyComponent*> StaticComps;
	UPaperTileMapComponent * TileMapComponent;
	UPaperTileMap * TileMap;
	UPaperTileLayer * TileLayer;
	if (!CheckData(TileMapActor, StaticComps))
	{
		return;
	}

	TileMapComponent = Cast<UPaperTileMapComponent>(TileMapActor->GetRenderComponent());
	TileMap          = TileMapComponent->TileMap;
	TileLayer        = (TileMap->TileLayers)[Layer];

	if (!TileLayer)
	{
		return;
	}

	FPaperTileInfo TileInfo = TileLayer->GetCell(PosX, PosY);
	StaticComps[0]->AddHiddenTile(Layer, PosX, PosY, TileInfo);
	TileInfo.TileSet = NULL;
	TileLayer->SetCell(PosX, PosY, TileInfo);
	TileMapComponent->MarkRenderStateDirty();

	if (RebuildCollision)
	{
		TileMap->RebuildCollision();
		TileMapComponent->RebuildCollision();
	}

} 

 void UK2Node_PixelTileHide::RestoreTileMap(APaperTileMapActor * TileMapActor)
 {
	 TArray<UPixel2DTileMapDestroyComponent*> StaticComps;
	 UPaperTileMapComponent * TileMapComponent;
	 UPaperTileMap * TileMap;

	 if (!CheckData(TileMapActor, StaticComps))
	 {
		 return;
	 }

	 TileMapComponent = Cast<UPaperTileMapComponent>(TileMapActor->GetRenderComponent());
	 TileMap = TileMapComponent->TileMap;

	 StaticComps[0]->RestoreHiddenTile(TileMap);
	 TileMapComponent->MarkRenderStateDirty();
 }

 void UK2Node_PixelTileHide::RebuildCollisionBox(APaperTileMapActor * TileMapActor)
 {
	 TArray<UPixel2DTileMapDestroyComponent*> StaticComps;
	 UPaperTileMapComponent * TileMapComponent;
	 UPaperTileMap * TileMap;

	 if (!CheckData(TileMapActor, StaticComps))
	 {
		 return;
	 }

	 TileMapComponent = Cast<UPaperTileMapComponent>(TileMapActor->GetRenderComponent());
	 TileMap = TileMapComponent->TileMap;

	 TileMap->RebuildCollision();
	 TileMapComponent->RebuildCollision();
 }

#undef LOCTEXT_NAMESPACE
