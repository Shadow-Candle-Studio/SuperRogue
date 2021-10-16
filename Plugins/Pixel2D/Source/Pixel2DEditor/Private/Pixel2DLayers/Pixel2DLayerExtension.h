// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Pixel2DLayerManager.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SUserWidget.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
#include "Runtime/Slate/Public/Widgets/Layout/SBox.h"
#include "Runtime/Slate/Public/Widgets/Views/SListView.h"
#include "Runtime/Slate/Public/Widgets/Input/SEditableText.h"


class FMenuBuilder;
class FPixel2DTableDelegate;
class FPixel2DLayerExtension;
class FPixel2DTableRow;

#define LOCTEXT_NAMESPACE "FPixel2DLayerTableRow"

class FPixel2DLayerExtension
{
public:
	FPixel2DLayerExtension();

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	void AddMenuExtension(FMenuBuilder& Builder);

	friend class FPixel2DTableRow;
private:

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	bool bRegistered = false;

	TArray<TSharedPtr<FPixel2DLayer>> Layers;
	TSharedPtr<FPixel2DTableRow> RowDelegate;
	TSharedPtr<SListView<TSharedPtr<FPixel2DLayer>>> LayerList;
	TSharedPtr<FPixel2DTableDelegate> TableDelegate;

	void AddLayer();
	void DeleteLayer(TSharedPtr<FPixel2DLayer> DeletedLayer);
	void ChangeDepth(TSharedPtr<FPixel2DLayer> Layer, bool Increment);

	TSharedRef<ITableRow> OnGenerateWidgetForList(TSharedPtr<FPixel2DLayer> InItem, const TSharedRef<STableViewBase>& OwnerTable);

};



class FPixel2DTableRow : public SMultiColumnTableRow<TSharedPtr<FPixel2DLayer>>
{

public:

   SLATE_BEGIN_ARGS(FPixel2DTableRow) : _RowLayerData() {}
		SLATE_ARGUMENT(TSharedPtr<FPixel2DLayer>, RowLayerData)
		SLATE_ARGUMENT(TSharedPtr<FPixel2DLayerExtension>, RowDelegate)
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs)
	{
		this->RowLayerData = InArgs._RowLayerData;
	};

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		this->RowLayerData = InArgs._RowLayerData;
		this->RowDelegate  = InArgs._RowDelegate;
		SMultiColumnTableRow<TSharedPtr<FPixel2DLayer>>::Construct(SMultiColumnTableRow<TSharedPtr<FPixel2DLayer>>::FArguments().Padding(1), InOwnerTableView);
	};

public:

	// SMultiColumnTableRow overrides

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
	{
		if (ColumnName == NAME_Depth)
		{
			return  SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(FMargin(2.0f, 0))
					[
						SNew(STextBlock)
						.Text(FText::Format(LOCTEXT("layermanager","{0}"),RowLayerData->LayerID))
					];
		}
		else if (ColumnName == NAME_Name)
		{
			return SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(FMargin(2.0f, 0.0f))
				[
					SAssignNew(NameTextBox, SEditableText)
					.Text(RowLayerData->Name)
					.IsReadOnly(true)
					.OnTextCommitted(this, &FPixel2DTableRow::EditLayerName)
				];

		}
		else if (ColumnName == NAME_Visible)
		{
			return  SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(FMargin(2.0f, 0.0f))
					[
						SNew(SCheckBox)
						.IsChecked(GetIsVisible(RowLayerData))
						.OnCheckStateChanged_Lambda(
							[this](ECheckBoxState NewState)
							{
								if (NewState == ECheckBoxState::Checked)
									RowLayerData->isVisible = true;
								else
									RowLayerData->isVisible = false;
							}
						)
					];
		}
		else if (ColumnName == NAME_Locked)
		{
			return  SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(FMargin(2.0f, 0.0f))
					[
						SNew(SCheckBox)
						.IsChecked(GetIsLocked(RowLayerData))
						.OnCheckStateChanged_Lambda(
							[this](ECheckBoxState NewState)
							{
								if (NewState == ECheckBoxState::Checked)
									RowLayerData->isLocked = true;
								else
									RowLayerData->isLocked = false;
							}
						)
					];
  
		}
		else if (ColumnName == NAME_Delete)
		{
			return  SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(FMargin(2.0f, 0.0f))
					[
						SNew(SButton)
						.Text(LOCTEXT("a2","delete"))
						.OnReleased_Lambda(
							[this]()
							{
								RowDelegate->DeleteLayer(RowLayerData);
							}
						)
					];
		}
		else if (ColumnName == NAME_DepthModifier)
		{
			return  SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(FMargin(2.0f, 0.0f))
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						.MaxHeight(12)
						[
							SNew(SButton)
							.Text(LOCTEXT("a+", "+"))
							.OnReleased_Lambda(
								[this]()
								{
									RowDelegate->ChangeDepth(RowLayerData, true);
								}
							)
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.MaxHeight(12)
						[
							SNew(SButton)
							.Text(LOCTEXT("a-", "-"))
							.OnReleased_Lambda(
								[this]()
								{
									RowDelegate->ChangeDepth(RowLayerData, false);
								}
							)
						]
					];
		}
		else
		{
			return SNullWidget::NullWidget;
		}

	}

public:
	static FName NAME_Name;
	static FName NAME_Visible;
	static FName NAME_Locked;
	static FName NAME_Delete;
	static FName NAME_DepthModifier;
	static FName NAME_Depth;

private:
	TSharedPtr<FPixel2DLayer>           RowLayerData;
	TSharedPtr<SEditableText>       NameTextBox;
	TSharedPtr<FPixel2DLayerExtension>  RowDelegate;

	void EditLayerName(const FText& CommitedText, ETextCommit::Type CommitType)
	{

		if (CommitType == ETextCommit::OnEnter)
		{
			NameTextBox->SetIsReadOnly(true);
		}
		else if(CommitType == ETextCommit::Default)
		{
			if (NameTextBox->IsTextReadOnly())
			{
				NameTextBox->SetIsReadOnly(false);
			}
		}
	}

	ECheckBoxState GetIsVisible(TSharedPtr<FPixel2DLayer> data)
	{
		if (data->isVisible)
			return ECheckBoxState::Checked;

		return ECheckBoxState::Unchecked;
	}

	ECheckBoxState GetIsLocked(TSharedPtr<FPixel2DLayer> data)
	{
		if (data->isLocked)
			return ECheckBoxState::Checked;

		return ECheckBoxState::Unchecked;
	}
};

#undef LOCTEXT_NAMESPACE


class FPixel2DTableDelegate : public STextBlock
{

private:
	TSharedPtr<FPixel2DLayerExtension> TableDelegate;

public:
	void Construct(const FArguments& InArgs, TSharedPtr<FPixel2DLayerExtension> _TableDelegate) {
		this->TableDelegate = _TableDelegate;
	};

public:

	TSharedRef<ITableRow> OnGenerateWidgetForList(TSharedPtr<FPixel2DLayer> InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		return  SNew(FPixel2DTableRow, OwnerTable)
				.RowLayerData(InItem)
				.RowDelegate(TableDelegate);
	}
};


