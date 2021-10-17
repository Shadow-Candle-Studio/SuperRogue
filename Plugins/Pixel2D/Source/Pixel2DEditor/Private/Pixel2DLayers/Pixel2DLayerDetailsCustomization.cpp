// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DLayerDetailsCustomization.h"
#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"

#include "Runtime/Slate/Public/Widgets/Input/STextComboBox.h"
#include "Runtime/Slate/Public/Widgets/Input/SButton.h"

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"

#include "Pixel2DLayerManager.h"

#define LOCTEXT_NAMESPACE "Pixel2DLayerCustomization"

TSharedRef<IPropertyTypeCustomization> FPixel2DLayerDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FPixel2DLayerDetailsCustomization());
}

void FPixel2DLayerDetailsCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	uint32 NumChildren;
	StructPropertyHandle->GetNumChildren(NumChildren);

	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const TSharedRef< IPropertyHandle > ChildHandle = StructPropertyHandle->GetChildHandle(ChildIndex).ToSharedRef();

		if (ChildHandle->GetProperty()->GetName() == TEXT("LayerName"))
		{
			Pixel2DLayerNameHandle = ChildHandle;
		}
		if (ChildHandle->GetProperty()->GetName() == TEXT("LayerDepthWithin"))
		{
			Pixel2DLayerDepthWithinHandle = ChildHandle;
		}
	}

	ComboBoxSrc.Empty();
	UPixel2DLayerManager::GetLayersByName(ComboBoxSrc);

	TSharedRef<STextComboBox> LayerNames = SNew(STextComboBox)
		.OptionsSource(&ComboBoxSrc)
		.OnSelectionChanged(this, &FPixel2DLayerDetailsCustomization::UpdateSelection);

	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget(LOCTEXT("Extra", "Pixel2D layer"))
		]
	.ValueContent()
		//.MinDesiredWidth(500)
		.MaxDesiredWidth(300)
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.MaxHeight(400)
				[
					LayerNames
				]

				+SVerticalBox::Slot()
				.MaxHeight(20)
				[
					SNew(SSpacer)
				]
			]
		];
}

void FPixel2DLayerDetailsCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}


void FPixel2DLayerDetailsCustomization::UpdateSelection(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
}

#undef LOCTEXT_NAMESPACE
