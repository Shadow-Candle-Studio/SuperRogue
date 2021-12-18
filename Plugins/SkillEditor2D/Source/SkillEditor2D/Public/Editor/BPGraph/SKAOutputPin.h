// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

/**
 * 
 */
class SKAOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SKAOutputPin){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	
	// End SGraphPin interface
	const FSlateBrush* GetPinBorder() const;
};