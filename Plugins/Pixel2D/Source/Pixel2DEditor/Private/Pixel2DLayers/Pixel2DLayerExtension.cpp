// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DLayerExtension.h"
#include "Pixel2DLayerCommands.h"
#include "Pixel2DLayerManager.h"

#include "Runtime/Slate/Public/Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"
#include "Runtime/Slate/Public/Widgets/Layout/SSpacer.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBox.h"
#include "Runtime/Slate/Public/Widgets/Views/SListView.h"

static const FName Pixel2DLayerTabName("Pixel2DLayerTab");
#define LOCTEXT_NAMESPACE "FPixel2DLayerExtension"

FName FPixel2DTableRow::NAME_Name(TEXT("axetablerow_name"));
FName FPixel2DTableRow::NAME_Visible(TEXT("axetablerow_visible"));
FName FPixel2DTableRow::NAME_Locked(TEXT("axetablerow_locked"));
FName FPixel2DTableRow::NAME_Delete(TEXT("axetablerow_delete"));
FName FPixel2DTableRow::NAME_Depth(TEXT("axetablerow_depth"));
FName FPixel2DTableRow::NAME_DepthModifier(TEXT("axetablerow_depthmodifier"));

FPixel2DLayerExtension::FPixel2DLayerExtension()
{
	bRegistered = false;
}

void FPixel2DLayerExtension::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FPixel2DLayerCommands::Get().OpenPluginWindow);

	if (!bRegistered) 
	{
		FGlobalTabmanager::Get()->RegisterTabSpawner(Pixel2DLayerTabName, FOnSpawnTab::CreateRaw(this, &FPixel2DLayerExtension::OnSpawnPluginTab))
			.SetDisplayName(LOCTEXT("FAxeLayerTabTitle", "AxeLayerTab"))
			.SetMenuType(ETabSpawnerMenuType::Hidden);
		bRegistered = true;
		RowDelegate = SNew(FPixel2DTableRow);
		TableDelegate = SNew(FPixel2DTableDelegate, MakeShareable<FPixel2DLayerExtension>(this));
	}
}

void FPixel2DLayerExtension::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(Pixel2DLayerTabName);
}

TSharedRef<SDockTab> FPixel2DLayerExtension::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FSlatModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("Slat.cpp"))
	);

	Layers = UPixel2DLayerManager::GetLayers();   
	
	SAssignNew(LayerList, SListView<TSharedPtr<FPixel2DLayer>>)
	.ItemHeight(27)
	.ListItemsSource(&Layers)
	.OnGenerateRow(TableDelegate.Get(), &FPixel2DTableDelegate::OnGenerateWidgetForList)
	.HeaderRow
	(
		SNew(SHeaderRow)
		+SHeaderRow::Column(FPixel2DTableRow::NAME_Depth)
		.DefaultLabel(LOCTEXT("axelayerextension_column_depth", "Depth"))
		.FillWidth(0.65f)

		+SHeaderRow::Column(FPixel2DTableRow::NAME_Name)
		.DefaultLabel(LOCTEXT("axelayerextension_column_name", "Name"))
		//.FillWidth(0.65f)

		+SHeaderRow::Column(FPixel2DTableRow::NAME_Visible)
		.FixedWidth(50.0f)
		.HAlignHeader(HAlign_Center)
		.VAlignHeader(VAlign_Center)
		.DefaultLabel(LOCTEXT("axelayerextension_column_visible", "Visible"))

		+SHeaderRow::Column(FPixel2DTableRow::NAME_Locked)
		.DefaultLabel(LOCTEXT("axelayerextension_column_locked", "Locked"))
		.FixedWidth(50.0f)

		+SHeaderRow::Column(FPixel2DTableRow::NAME_Delete)
		.DefaultLabel(LOCTEXT("axelayerextension_column_delete", "Delete"))
		.FixedWidth(50.0f)

		+SHeaderRow::Column(FPixel2DTableRow::NAME_DepthModifier)
		.DefaultLabel(LOCTEXT("axelayerextension_column_depthmodifier", "Order"))
		.FixedWidth(50.0f)

	);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.MaxHeight(400)
				[
					SNew(SBorder)
					.Padding(0)
					.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
					[
						LayerList.ToSharedRef()
					]
				]
				+SVerticalBox::Slot()
				.MaxHeight(20)
				[
					SNew(SSpacer)
				]
				+SVerticalBox::Slot()
				.Padding(FMargin(10.0f, 10.0f))
				.AutoHeight()
				[
					SNew(SButton)    
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("WindowWidgetText", "Add new layer"))
					.OnReleased(FSimpleDelegate::CreateRaw(this, &FPixel2DLayerExtension::AddLayer))
				 ]
		]
		];
}


void FPixel2DLayerExtension::AddLayer()
{
	UPixel2DLayerManager::AddLayer();
	Layers = UPixel2DLayerManager::GetLayers();
	LayerList->RebuildList();
}

void FPixel2DLayerExtension::DeleteLayer(TSharedPtr<FPixel2DLayer> DeletedLayer)
{
	UPixel2DLayerManager::DeleteLayer(DeletedLayer);
	Layers = UPixel2DLayerManager::GetLayers();
	LayerList->RebuildList();
}

void FPixel2DLayerExtension::ChangeDepth(TSharedPtr<FPixel2DLayer> Layer, bool Increment)
{
	UPixel2DLayerManager::ChangeDepth(Layer, Increment);
	Layers = UPixel2DLayerManager::GetLayers();
	LayerList->RebuildList();
}

TSharedRef<ITableRow> FPixel2DLayerExtension::OnGenerateWidgetForList(TSharedPtr<FPixel2DLayer> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{ 
	return SNew(FPixel2DTableRow, OwnerTable);
}

#undef LOCTEXT_NAMESPACE