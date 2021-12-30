// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorPreviewSummoner.h"

#include "BlueprintEditorTabs.h"
#include "SkillAssetEditor.h"

#define LOCTEXT_NAMESPACE "SkillEditorPreviewSummoner"

SkillEditorPreviewSummoner::SkillEditorPreviewSummoner
(TSharedPtr<FSkillAssetEditor> InSKillEditorPtr):FWorkflowTabFactory
(FSkillAssetEditor::PreviewTabId,InSKillEditorPtr),
SKillEditor(InSKillEditorPtr)
{
	TabLabel=LOCTEXT("SkillAssetPreviewLabel","Preview");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),
	 		"LevelEditor.Tabs.Details");
	bIsSingleton=true;
	ViewMenuDescription=LOCTEXT("SkillAssetPreviewDescription","Preview");
	ViewMenuTooltip=LOCTEXT("SkillAssetPreview_ToolTip","Show the preview tab");
	SAssignNew(SkillAssetPreviewTabBody,SkillEditorPreviewTabBody);
}

TSharedRef<SWidget> SkillEditorPreviewSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SkillAssetPreviewTabBody.ToSharedRef();
}

FText SkillEditorPreviewSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillAssetEditorPreviewTabToolTip","This preview tab allows displaying paper animaton with complex particle effects");
}

TSharedPtr<SkillEditorPreviewTabBody>& SkillEditorPreviewSummoner::GetFactoryTabBody()
{
	return SkillAssetPreviewTabBody;
}


SkillEditorPreviewSummoner::~SkillEditorPreviewSummoner()
{
}


#undef LOCTEXT_NAMESPACE