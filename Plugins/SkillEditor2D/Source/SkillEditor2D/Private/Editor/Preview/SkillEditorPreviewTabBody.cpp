// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorPreviewTabBody.h"


void SkillEditorPreviewTabBody::Construct(const FArguments& Inargs)
{
	m_viewportWidget=SNew(SKillEditorPreviewViewport);
	
	TSharedPtr<SVerticalBox> ViewportContainer=nullptr;
	this->ChildSlot
	[
		SAssignNew(ViewportContainer,SVerticalBox)+
		SVerticalBox::Slot()
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.FillHeight(1)
		[
			SNew(SOverlay)
			//the viewport
			+SOverlay::Slot()
			[
				m_viewportWidget.ToSharedRef()

			]
		]

	];
}

SkillEditorPreviewTabBody::SkillEditorPreviewTabBody()
{
}

SkillEditorPreviewTabBody::~SkillEditorPreviewTabBody()
{
}

TSharedPtr<SKillEditorPreviewViewport>& SkillEditorPreviewTabBody::GetSKAViewport()
{
	return m_viewportWidget;
}
