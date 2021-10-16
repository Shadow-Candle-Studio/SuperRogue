// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UK2Node_PixelTileHide.generated.h"


class FBlueprintActionDatabaseRegistrar;
class UPaperTileMap;
class APaperTileMapActor;
class UPaperTileMapComponent;
class UPixel2DTileMapDestroyComponent;

UENUM(BlueprintType)
enum class EPixel2DTileDestoryError : uint8
{
	E_OK,
	E_INVALID_ACTOR,
	E_INVALID_TILEMAP,
	E_INVALID_TILEDESTORY,
	E_INVALID_INDEX
};

UCLASS()
class PIXEL2D_API UK2Node_PixelTileHide : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Pixel2D|TileDestroy", meta = (AdvancedDisplay = 4))
	static void HideActorTile(APaperTileMapActor * TileMapActor, int Layer, int PosX, int PosY, bool RebuildCollision = false);

	UFUNCTION(BlueprintCallable, Category = "Pixel2D|TileDestroy")
	static void  RestoreTileMap(APaperTileMapActor * TileMapActor);

	UFUNCTION(BlueprintCallable, Category = "Pixel2D|TileDestroy")
	static void  RebuildCollisionBox(APaperTileMapActor * TileMapActor);


private:
	static bool CheckData(const APaperTileMapActor * InTileMapActor, TArray<UPixel2DTileMapDestroyComponent*> & OutStaticComps);
};
