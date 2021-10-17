// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DTileReplacer.h"
#include "Paper2D/Classes/PaperTileSet.h"
#include "Paper2D/Classes/PaperTileMap.h"
#include "Paper2D/Classes/PaperTileLayer.h"
#include "Paper2D/Classes/PaperTileMapActor.h"
#include "Paper2D/Classes/PaperTileMapComponent.h"
#include "Paper2D/Classes/PaperFlipbook.h"
#include "Paper2D/Classes/PaperFlipbookComponent.h"
#include "Paper2D/Classes/PaperFlipbookActor.h"

#include "Editor/UnrealEd/Public/Editor.h"
#include "Editor/UnrealEd/Public/ActorGroupingUtils.h"

FPixel2DTileReplacer::FPixel2DTileReplacer()
	:bAlreadyProcessing(false),
	 MappingTable(nullptr)
{
	MappingTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("DataTable'/Game/Pixel2D/Blueprints/AnimatedTileReplacer/Pixel2DTileReplacerDataTable.Pixel2DTileReplacerDataTable'")));
	if (MappingTable)
	{
		MappingTable->AddToRoot();
	}
}

FPixel2DTileReplacer::~FPixel2DTileReplacer()
{
	if (MappingTable)
	{
		MappingTable->RemoveFromRoot();
	}
}

void FPixel2DTileReplacer::Initialize()
{
	if (MappingTable == nullptr)
	{
		return;
	}

	RegisterActorSpawnHandler();
}

void FPixel2DTileReplacer::Uninitialize()
{}

void FPixel2DTileReplacer::RegisterActorSpawnHandler()
{
	FEditorDelegates::OnNewActorsDropped.AddRaw(this, &FPixel2DTileReplacer::OnActorDragged);
}

void FPixel2DTileReplacer::OnActorDragged(const TArray<UObject*>& InObj, const TArray<AActor*>& InActor)
{
	if (bAlreadyProcessing)
	{
		// We dont allow replacing tiles with other tilemaps
		return;
	}
	bAlreadyProcessing = true;
	
	APaperTileMapActor * TileMapActor;

	for (auto CrtObj : InActor)
	{
		AddedActors.Empty();
		TileMapActor = Cast<APaperTileMapActor>(CrtObj);

		if (TileMapActor && TileMapActor->GetRenderComponent())
		{
			if (!HasReplaceableTiles(TileMapActor->GetRenderComponent()->TileMap))
			{
				continue;
			}
			TileMapActor->GetRenderComponent()->Modify();
			// Avoid marking the tilemap as editable whenever possible as there is a bug in Paper2D
			// that causes the tool to crash when running with the debugger (although there's no crash
			// in the standalone/released versions)
			TileMapActor->GetRenderComponent()->MakeTileMapEditable();
			UPaperTileMap *TileMap = TileMapActor->GetRenderComponent()->TileMap;

			for (UPaperTileLayer* CrtLayer : TileMap->TileLayers)
			{
				for (int32 i = 0; i < CrtLayer->GetLayerWidth(); i++)
				{
					for (int32 j = 0; j < CrtLayer->GetLayerHeight(); j++)
					{
						MappingTable->ForeachRow<FPixel2DTileReplacerRow>("ctx", [this, i,j, CrtLayer, TileMapActor, TileMap](const FName& Key, const FPixel2DTileReplacerRow& Value)
						{
							FPaperTileInfo TileInfo = CrtLayer->GetCell(i, j);
							if (ShouldReplaceTile(TileInfo, &Value))
							{
								FVector TileRelativePosition = TileMap->GetTileCenterInLocalSpace(i, j, CrtLayer->GetLayerIndex());
								ReplaceTile(TileInfo, &Value, TileMapActor, TileRelativePosition);
								TileInfo.TileSet = NULL;
								CrtLayer->SetCell(i, j, TileInfo);
							}
						});
					}
				}
			}
		}

		if (AddedActors.Num() != 0)
		{
			AddedActors.Add(TileMapActor);
			UActorGroupingUtils::Get()->GroupActors(AddedActors);
		}
	}

	bAlreadyProcessing = false;
}

bool FPixel2DTileReplacer::ShouldReplaceTile(const FPaperTileInfo & TileInfo, const FPixel2DTileReplacerRow * TableRow)
{
	if (TableRow == nullptr)
	{
		return false;
	}
	if (TileInfo.TileSet == nullptr)
	{
		return false;
	}

	int32 ExpectedTileIndex = TableRow->TileSetIndex;
	int32 ActualTileIndex = TileInfo.GetTileIndex();

	FString ExpectedTileSetName = TableRow->TileSet.GetAssetName();
	FString ActualTileSetName = ((TileInfo.TileSet)->GetName());

	return ((ExpectedTileIndex == ActualTileIndex) && (ExpectedTileSetName == ActualTileSetName));
}


void FPixel2DTileReplacer::ReplaceTile(const FPaperTileInfo & TileInfo, const FPixel2DTileReplacerRow * TableRow, APaperTileMapActor* Actor, const FVector &TilePosition)
{
	if (!Actor || !TableRow)
	{
		return;
	}
	
	UPaperFlipbook * NewTileFlipbook = TableRow->Flipbook.LoadSynchronous();
	if (!NewTileFlipbook)
	{
		return;
	}

	if (TableRow->bSpawnActor)
	{
		AActor * NewActor = SpawnActor(Actor, TilePosition, NewTileFlipbook, TableRow->bLockActorRelativePosition);
		if (TableRow->bLockActorRelativePosition && NewActor)
		{
			AddedActors.Add(NewActor);
		}
	}
	else
	{
		SpawnComponent(Actor, TilePosition, NewTileFlipbook);
	}
}

APaperFlipbookActor* FPixel2DTileReplacer::SpawnActor(APaperTileMapActor* Actor, const FVector &TileCenterPosition, UPaperFlipbook * Flipbook, bool bLockPosition)
{
	if (!Actor || !Flipbook)
	{
		return NULL;
	}

	// Find the absolute coordinates in the Actor's world
	USceneComponent * RootTileMapComponent = Actor->GetRootComponent();
	UPaperTileMapComponent * TileMapComponent = Cast<UPaperTileMapComponent>(RootTileMapComponent);
	if (!TileMapComponent)
	{
		return NULL;
	}

	FVector MapAbsolutePosition = TileMapComponent->GetComponentTransform().TransformPosition(TileCenterPosition);

	APaperFlipbookActor * NewFlipbookActor = NULL;
	UWorld * ActorWorld = Actor->GetWorld();
		
	FActorSpawnParameters NewActorSpawnParams;

	NewFlipbookActor = ActorWorld->SpawnActor<APaperFlipbookActor>(MapAbsolutePosition,FRotator::ZeroRotator, NewActorSpawnParams);
	if (NewFlipbookActor)
	{
		UPaperFlipbookComponent* RenderComponent = NewFlipbookActor->GetRenderComponent();
		check(RenderComponent);

		RenderComponent->UnregisterComponent();
		RenderComponent->SetFlipbook(Flipbook);
		RenderComponent->RegisterComponent();
	}

	return NewFlipbookActor;
}

void FPixel2DTileReplacer::SpawnComponent(APaperTileMapActor* Actor, const FVector &TilePosition, UPaperFlipbook * Flipbook)
{
	if (!Actor || !Flipbook)
	{
		return;
	}
	 
	USceneComponent * RenderComponent = Actor->GetRootComponent();

	UPaperFlipbookComponent * SpawnedComponent = NewObject<UPaperFlipbookComponent>(Actor);
	SpawnedComponent->SetFlipbook(Flipbook);
	SpawnedComponent->AttachToComponent(RenderComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SpawnedComponent->SetRelativeLocation(TilePosition);

	Actor->AddInstanceComponent(SpawnedComponent);
	SpawnedComponent->OnComponentCreated();
	SpawnedComponent->RegisterComponent();
}

bool FPixel2DTileReplacer::HasReplaceableTiles(const UPaperTileMap * TileMap)
{
	bool bTileMapPresent = false;
	MappingTable->ForeachRow<FPixel2DTileReplacerRow>("ctx", [this, &bTileMapPresent, TileMap](const FName& Key, const FPixel2DTileReplacerRow& Value)
	{
		FString RowTileMapName = Value.TileMap.GetAssetName();
		FString TileMapName    = TileMap->GetName();
		if (RowTileMapName == TileMapName)
		{
			bTileMapPresent = true;
		}
	});

	return bTileMapPresent;
}
