// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Components/ActorComponent.h"

#include "Paper2D/Classes/PaperTileSet.h"
#include "Paper2D/Classes/PaperTileMap.h"
#include "Paper2D/Classes/PaperTileLayer.h"
#include "Paper2D/Classes/PaperTileMapComponent.h"

#include "Pixel2DTileMapDestroyComponent.generated.h"

struct PIXEL2D_API FPixel2DTileMapDestroyedCell
{
	int32 Layer;
	int32 PosX;
	int32 PosY;

	FPixel2DTileMapDestroyedCell(int32 Layer, int32 PosX, int32 PosY)
		: Layer(Layer), PosX(PosX), PosY(PosY)
	{}

	bool operator==(const FPixel2DTileMapDestroyedCell& Other) const
	{
		return (Layer == Other.Layer) && (PosX == Other.PosX) && (PosY == Other.PosY);
	}

	friend inline int32 GetTypeHash(const FPixel2DTileMapDestroyedCell& Handle)
	{
		uint32 RetVal = HashCombine(GetTypeHash(Handle.PosX), GetTypeHash(Handle.PosY));
		RetVal        = HashCombine(GetTypeHash(Handle.Layer), RetVal);
		return RetVal;
	}
};

UCLASS(ClassGroup = Pixel2D, meta = (BlueprintSpawnableComponent))
class PIXEL2D_API UPixel2DTileMapDestroyComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:

	void AddHiddenTile(int32 Layer, int32 PosX, int32 PosY, FPaperTileInfo &TileInfo)
	{
		if ((Layer < 0) || (PosX < 0) || (PosY < 0))
		{
			return;
		}

		FPixel2DTileMapDestroyedCell Cell(Layer, PosX, PosY);
		if (!HiddenCells.Find(Cell))
		{
			HiddenCells.Add(Cell, TileInfo);
		}
	}

	void RestoreHiddenTile(UPaperTileMap * TileMap)
	{
		if (HiddenCells.Num() == 0)
		{
			return;
		}

		if ((!TileMap) || (TileMap->TileLayers.Num() == 0))
		{
			return;
		}

		for (auto & Elem : HiddenCells)
		{
			FPixel2DTileMapDestroyedCell TilePos = Elem.Key;
			FPaperTileInfo TileInfo = Elem.Value;

			if (TileMap->TileLayers.Num() > TilePos.Layer)
			{
				TileMap->TileLayers[TilePos.Layer]->SetCell(TilePos.PosX, TilePos.PosY, TileInfo);
			}
		}

		HiddenCells.Empty();
	}

	// ActorComponent Overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

private:
	TMap<FPixel2DTileMapDestroyedCell, FPaperTileInfo> HiddenCells;

	void RestoreCollision();
};
