// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillEditorPreviewClient.h"
#include "SKillEditorPreviewViewport.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillEditorPreviewTabBody: public
 SCompoundWidget
{
	SLATE_BEGIN_ARGS(SkillEditorPreviewTabBody){}
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& Inargs);
	SkillEditorPreviewTabBody();
	~SkillEditorPreviewTabBody();
private:
	TSharedPtr<FEditorViewportClient> m_PreviewClient;
	TSharedPtr<SKillEditorPreviewViewport> m_viewportWidget;
};
