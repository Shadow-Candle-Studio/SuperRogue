// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditorSequenceTabSummoner.h"

#include "ISequencerModule.h"
#include "SkillAsset.h"
#include "SkillAssetEditor.h"
#include "WidgetAnimation.h"
#define LOCTEXT_NAMESPACE "SkillEditorSequenceSummoner"

class UWidgetAnimation;

SkillAssetEditorSequenceTabSummoner::SkillAssetEditorSequenceTabSummoner(TSharedPtr<FSkillAssetEditor> InEditorPtr):
FWorkflowTabFactory(FSkillAssetEditor::SequencerAreaTabID,InEditorPtr),EditorPtr(InEditorPtr)
{
	check(InEditorPtr.IsValid())
	TabLabel=LOCTEXT("Sequence you can edit", "SequenceOfSkillAsset");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),
	 			"LevelEditor.Tabs.Details");
	bIsSingleton=true;
	ViewMenuDescription = LOCTEXT("Sequnce you can edit your skill", "SequenceOfSkillAsset");
	ViewMenuTooltip = LOCTEXT("Where you can edit curve and add custom events", "SequenceOfSkillAsset");
	

	

		
	
}

TSharedRef<SWidget> SkillAssetEditorSequenceTabSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(EditorPtr.Pin().IsValid())


	
	return SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
		EditorPtr.Pin()->GetSkillAssetSequencerWidget().ToSharedRef()
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