// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorPreviewSummoner.h"

#include "BlueprintEditorTabs.h"
#include "SkillAssetEditor.h"

#define LOCTEXT_NAMESPACE "SkillEditorPreviewSummoner"
const FName PreviewTabID="SkillEditorPreview";
SkillEditorPreviewSummoner::SkillEditorPreviewSummoner
(TSharedPtr<FSkillAssetEditor> InSKillEditorPtr):FWorkflowTabFactory(PreviewTabID,InSKillEditorPtr),SKillEditor(InSKillEditorPtr)
{
	TabLabel=LOCTEXT("SkillAssetPreviewLabel","Preview");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),"Kismet.Tabs.Components");
	bIsSingleton=true;
	ViewMenuDescription=LOCTEXT("SkillAssetPreviewDescription","Preview");
	ViewMenuTooltip=LOCTEXT("SkillAssetPreview_ToolTip","Show the preview tab");
}

TSharedRef<SWidget> SkillEditorPreviewSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SKillEditor.Pin()->SpawnPreviewWidget();
}

FText SkillEditorPreviewSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillAssetEditorPreviewTabToolTip","This preview tab allows displaying paper animaton with complex particle effects");
}



SkillEditorPreviewSummoner::~SkillEditorPreviewSummoner()
{
}


#undef LOCTEXT_NAMESPACE