// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetBPGraphTabSummoner.h"

#include "SkillAssetEditor.h"
#define LOCTEXT_NAMESPACE "SkillAssetBPGraphTabSummoner"

SkillAssetBPGraphTabSummoner::SkillAssetBPGraphTabSummoner(TSharedPtr<FSkillAssetEditor> InEditorPtr):
FWorkflowTabFactory(FSkillAssetEditor::GraphCanvasId,InEditorPtr),EditorPtr(InEditorPtr)
{
	TabLabel=LOCTEXT("Events we added in this asset", "Events");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),
		"GraphEditor.EventGraph_16x");
	bIsSingleton=true;
	ViewMenuDescription = LOCTEXT("All of the events", "Events");
	ViewMenuTooltip = LOCTEXT("where we add our customized events", "Show all of the events");
	
}

TSharedRef<SWidget> SkillAssetBPGraphTabSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(STextBlock).Text(FText::FromString("GraphArea"))
		];
}

FText SkillAssetBPGraphTabSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillAssetEditorBPGraphTabToolTip","This BP Graph shows you all of the events you have added to this asset");
}


SkillAssetBPGraphTabSummoner::~SkillAssetBPGraphTabSummoner()
{
}


#undef LOCTEXT_NAMESPACE