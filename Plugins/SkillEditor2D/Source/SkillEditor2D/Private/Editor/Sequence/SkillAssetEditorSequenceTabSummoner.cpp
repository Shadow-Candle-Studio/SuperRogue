// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditorSequenceTabSummoner.h"

#include "SkillAssetEditor.h"
#define LOCTEXT_NAMESPACE "SkillEditorSequenceSummoner"

SkillAssetEditorSequenceTabSummoner::SkillAssetEditorSequenceTabSummoner(TSharedPtr<FSkillAssetEditor> InEditorPtr):
FWorkflowTabFactory(FSkillAssetEditor::SequencerAreaTabID,InEditorPtr),EditorPtr(InEditorPtr)
{
	TabLabel=LOCTEXT("Sequence you can edit", "Sequence");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),
	 			"LevelEditor.Tabs.Details");
	bIsSingleton=true;
	ViewMenuDescription = LOCTEXT("Sequnce you can edit your skill", "Sequence");
	ViewMenuTooltip = LOCTEXT("Where you can edit curve and add custom events", "Sequce player");
}

TSharedRef<SWidget> SkillAssetEditorSequenceTabSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(STextBlock).Text(FText::FromString("Sequence"))
		];
}

FText SkillAssetEditorSequenceTabSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillAssetEditorSequenceTabToolTip","I this sequence alike module you can add any peroperties or events you would like");
}

SkillAssetEditorSequenceTabSummoner::~SkillAssetEditorSequenceTabSummoner()
{
}
#undef LOCTEXT_NAMESPACE