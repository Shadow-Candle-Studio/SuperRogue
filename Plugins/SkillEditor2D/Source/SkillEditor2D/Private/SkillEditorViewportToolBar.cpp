// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorViewportToolBar.h"
#include "SKillEditorPreviewViewport.h"

void SkillEditorViewportToolBar::Construct(const FArguments& InArgs,
	TSharedPtr<SKillEditorPreviewViewport> InRealViewport)
{
	SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(),InRealViewport);
}

SkillEditorViewportToolBar::SkillEditorViewportToolBar()
{
}

SkillEditorViewportToolBar::~SkillEditorViewportToolBar()
{
}
