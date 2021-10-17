// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DLayerManager.h"

#define LOCTEXT_NAMESPACE "FPixel2DLayerManager"

TArray<TSharedPtr<FPixel2DLayer>> UPixel2DLayerManager::Layers;
bool UPixel2DLayerManager::isVisible = true;

UPixel2DLayerManager::UPixel2DLayerManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPixel2DLayerManager::AddLayer()
{
	FText NewName = FText::Format(LOCTEXT("layermanager","newlayername{0}"), FindDefaultDuplicates());
	uint8 NewLayerID = FindAvailableID();

	TSharedPtr<FPixel2DLayer> NewLayer(new FPixel2DLayer(NewName,NewLayerID));
	UPixel2DLayerManager::Layers.Add(NewLayer);
}

void UPixel2DLayerManager::DeleteLayer(TSharedPtr<FPixel2DLayer> DeletedLayer)
{
	uint8 DeletedLayerID = DeletedLayer->LayerID;
	Layers.RemoveSingle(DeletedLayer);

	for (auto Layer : Layers)
		if (Layer->LayerID > DeletedLayerID)
			Layer->LayerID--;
}

void UPixel2DLayerManager::ChangeDepth(TSharedPtr<FPixel2DLayer> Layer, bool Increment)
{
	if ((Increment && (Layer->LayerID == 0)) ||
		(!Increment && (Layer->LayerID == Layers.Num()-1)))
		return;

	uint8 OldLayerID = Layer->LayerID;
	uint8 NewLayerID = (Increment ? Layer->LayerID-1 : Layer->LayerID+1);
	int32 OldIndex = -1;
	int32 NewIndex = -1;


	for (int i = 0; i < Layers.Num(); i++)
	{
		if (Layers[i]->LayerID == NewLayerID)
		{
			Layers[i]->LayerID = OldLayerID;
			OldIndex = i;
		}
		if (Layers[i] == Layer)
			NewIndex = i;
	}

	Layer->LayerID = NewLayerID;

	Layers.Swap(OldIndex, NewIndex);
}

int UPixel2DLayerManager::GetNumLayers()
{
	return UPixel2DLayerManager::Layers.Num();
}

TArray<TSharedPtr<FPixel2DLayer>> UPixel2DLayerManager::GetLayers()
{
	return Layers;
}

bool UPixel2DLayerManager::GetIsVisible(TSharedPtr<FPixel2DLayer> LayerName)
{
	return isVisible;
}

void UPixel2DLayerManager::SetIsVisible(bool bVisible)
{
	isVisible = bVisible;
}

bool UPixel2DLayerManager::GetIsLocked(TSharedPtr<FPixel2DLayer> LayerName)
{
	return false;
}

uint16 UPixel2DLayerManager::FindDefaultDuplicates()
{
	return Layers.Num()+1;
}

uint8 UPixel2DLayerManager::FindAvailableID()
{
	bool UsedIDs[255];
	
	for (int i = 0; i < 255; i++)
		UsedIDs[i] = false;

	for (auto Layer : Layers)
	{
		UsedIDs[Layer->LayerID] = true;
	}

	for (uint8 i = 0; i < 255; i++)
		if (UsedIDs[i] == false)
			return i;

	return 0;
}

void UPixel2DLayerManager::GetLayersByName(TArray<TSharedPtr<FString>> &Container)
{
	for (auto Layer : Layers)
	{
		TSharedPtr<FString> LayerName(new FString((Layer->Name).ToString()));
		Container.Add(LayerName);
	}
}

#undef LOCTEXT_NAMESPACE
