// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"

class FPixel2DLayerDetailsCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	TSharedPtr<IPropertyHandle> Pixel2DLayerNameHandle;
	TSharedPtr<IPropertyHandle> Pixel2DLayerDepthWithinHandle;

	void UpdateSelection(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);

private:
	TArray<TSharedPtr<FString>> ComboBoxSrc;
};