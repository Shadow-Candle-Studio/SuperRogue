// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
/**
 * 
 */
class SKILLEDITOR2D_API SkillEditorWindowStyle
{
public:
	static void Initialize();

	static void ShutDown();

	static void Reloadtextures();

	static const ISlateStyle& Get();
	static FName GetStyleSetName();
private:
	static TSharedRef<FSlateStyleSet> Create();



	static TSharedPtr< FSlateStyleSet > StyleInstance;
	SkillEditorWindowStyle();
	~SkillEditorWindowStyle();
};
