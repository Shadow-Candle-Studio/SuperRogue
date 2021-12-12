// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetPropertyTabSummoner.h"

#include "SkillAssetEditor.h"
#define LOCTEXT_NAMESPACE "SkillAssetPropertyTabSummoner"

SkillAssetPropertyTabSummoner::SkillAssetPropertyTabSummoner(TSharedPtr<FSkillAssetEditor> InEditorPtr):FWorkflowTabFactory(FSkillAssetEditor::PreviewTabId, InEditorPtr)
{
	TabLabel=LOCTEXT("Details with preview", "Details");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),"Kismet.Tabs.Components");
	bIsSingleton=true;
	ViewMenuDescription = LOCTEXT("Details with preview", "Details");
	ViewMenuTooltip = LOCTEXT("Details with preview_ToolTip", "Show the details view");
}

TSharedRef<SWidget> SkillAssetPropertyTabSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(InEditorPtr.IsValid());
	return InEditorPtr.Pin()->SpawnDetailPanelWidget();
}

FText SkillAssetPropertyTabSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FWorkflowTabFactory::GetTabToolTipText(Info);
}


SkillAssetPropertyTabSummoner::~SkillAssetPropertyTabSummoner()
{
}
#undef LOCTEXT_NAMESPACE