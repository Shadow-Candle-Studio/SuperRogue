// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCommonEditorViewportToolbarBase.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillEditorViewportToolBar: public SCommonEditorViewportToolbarBase
{
public:
	SLATE_BEGIN_ARGS(SkillEditorViewportToolBar){}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs, TSharedPtr<class SKillEditorPreviewViewport> InRealViewport);
	SkillEditorViewportToolBar();
	~SkillEditorViewportToolBar();
};
