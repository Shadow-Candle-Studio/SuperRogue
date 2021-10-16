// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once


#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"

#include "Pixel2DLayerManager.generated.h"

USTRUCT()
struct FPixel2DLayer
{
	FPixel2DLayer() {};

	FPixel2DLayer(FText LayerName, uint8 LayerID)
		: Name(LayerName), LayerID(LayerID)
	{};

	GENERATED_BODY()

	UPROPERTY(Category = PixelLayer, EditAnywhere)
	FText Name;

	UPROPERTY(Category = PixelLayer, EditAnywhere)
	uint8 LayerID;

	UPROPERTY(Category = PixelLayer, EditAnywhere)
	bool isVisible = true;

	UPROPERTY(Category = PixelLayer, EditAnywhere)
	bool isLocked = false;

};

UCLASS()
class PIXEL2D_API UPixel2DLayerManager : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	static void AddLayer();
	static void DeleteLayer(TSharedPtr<FPixel2DLayer> DeletedLayer);
	static void ChangeDepth(TSharedPtr<FPixel2DLayer> Layer, bool Increment);

	static int  GetNumLayers();
	static TArray<TSharedPtr<FPixel2DLayer>> GetLayers();
	static bool GetIsVisible(TSharedPtr<FPixel2DLayer> LayerName);
	static bool GetIsLocked(TSharedPtr<FPixel2DLayer> LayerName);

	static void SetIsVisible(bool bVisible);

	static void GetLayersByName(TArray<TSharedPtr<FString>> &Container);

public:
	static TArray<TSharedPtr<FPixel2DLayer>> Layers;
	static bool isVisible;

private:
	static uint16 FindDefaultDuplicates();
	static uint8  FindAvailableID();
};
