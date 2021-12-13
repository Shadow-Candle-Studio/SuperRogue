// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SKILLEDITOR2D_API SKAUEdGraphTab : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SKAUEdGraphTab){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TSharedPtr<SGraphEditor> GraphEditorPtr;
	class USKAUEdGraph* GraphObj;
	SKAUEdGraphTab();
	~SKAUEdGraphTab();
};
