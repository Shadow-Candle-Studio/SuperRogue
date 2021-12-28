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
	TabLabel=LOCTEXT("Sequence you can edit", "Sequence");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),
	 			"LevelEditor.Tabs.Details");
	bIsSingleton=true;
	ViewMenuDescription = LOCTEXT("Sequnce you can edit your skill", "Sequence");
	ViewMenuTooltip = LOCTEXT("Where you can edit curve and add custom events", "Sequce player");
	

	const float InTime  = 0.f;
		const float OutTime = 5.0f;

		// FSequencerViewParams ViewParams(TEXT("MysequencerSetting"));
		// {
		// 	ViewParams.OnGetAddMenuContent = FOnGetAddMenuContent::CreateSP(this, &SkillAssetEditorSequenceTabSummoner::OnGetAnimationAddMenuContent);
		//
		//     ViewParams.OnBuildCustomContextMenuForGuid = FOnBuildCustomContextMenuForGuid::CreateSP(this, &SkillAssetEditorSequenceTabSummoner::OnBuildCustomContextMenuForGuid);
		// }

		FSequencerInitParams SequencerInitParams;
		{
			UWidgetAnimation* NullAnimation = UWidgetAnimation::GetNullAnimation();
			FFrameRate TickResolution = NullAnimation->MovieScene->GetTickResolution();
			FFrameNumber StartFrame = (InTime  * TickResolution).FloorToFrame();
			FFrameNumber EndFrame   = (OutTime * TickResolution).CeilToFrame();
			NullAnimation->MovieScene->SetPlaybackRange(StartFrame, (EndFrame-StartFrame).Value);
			FMovieSceneEditorData& EditorData = NullAnimation->MovieScene->GetEditorData();
			EditorData.WorkStart = InTime;
			EditorData.WorkEnd   = OutTime;

			//SequencerInitParams.ViewParams = ViewParams;
			SequencerInitParams.RootSequence = NullAnimation;
			SequencerInitParams.bEditWithinLevelEditor = false;
			SequencerInitParams.ToolkitHost = InEditorPtr->GetToolkitHost();
			// SequencerInitParams.PlaybackContext = TAttribute<UObject*>(this, &SkillAssetEditorSequenceTabSummoner::GetAnimationPlaybackContext);
			// SequencerInitParams.EventContexts = TAttribute<TArray<UObject*>>(this, &SkillAssetEditorSequenceTabSummoner::GetAnimationEventContexts);

			SequencerInitParams.HostCapabilities.bSupportsCurveEditor = true;
		};

		TabSequencer = FModuleManager::LoadModuleChecked<ISequencerModule>("Sequencer").CreateSequencer(SequencerInitParams);
		// // Never recompile the blueprint on evaluate as this can create an insidious loop
		// TabSequencer->GetSequencerSettings()->SetCompileDirectorOnEvaluate(false);
		// TabSequencer->OnMovieSceneDataChanged().AddSP( this, &FWidgetBlueprintEditor::OnMovieSceneDataChanged );
		// TabSequencer->OnMovieSceneBindingsPasted().AddSP( this, &FWidgetBlueprintEditor::OnMovieSceneBindingsPasted );
		// // Change selected widgets in the sequencer tree view
		// TabSequencer->GetSelectionChangedObjectGuids().AddSP(this, &FWidgetBlueprintEditor::SyncSelectedWidgetsWithSequencerSelection);
		// OnSelectedWidgetsChanged.AddSP(this, &FWidgetBlueprintEditor::SyncSequencerSelectionToSelectedWidgets);
		//
		// // Allow sequencer to test which bindings are selected
		// TabSequencer->OnGetIsBindingVisible().BindRaw(this, &FWidgetBlueprintEditor::IsBindingSelected);

		
	
}

TSharedRef<SWidget> SkillAssetEditorSequenceTabSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(SBox)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			TabSequencer->GetSequencerWidget()
		];
}

FText SkillAssetEditorSequenceTabSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillAssetEditorSequenceTabToolTip","I this sequence alike module you can add any peroperties or events you would like");
}

void SkillAssetEditorSequenceTabSummoner::OnGetAnimationAddMenuContent( FMenuBuilder& , TSharedRef<ISequencer>)
{
	;
}

void SkillAssetEditorSequenceTabSummoner::OnBuildCustomContextMenuForGuid(FMenuBuilder&, FGuid)
{
}

UObject* SkillAssetEditorSequenceTabSummoner::GetAnimationPlaybackContext() const
{
	return EditorPtr.Pin()->GetSkillAsset();
}

TArray<UObject*> SkillAssetEditorSequenceTabSummoner::GetAnimationEventContexts() const
{
	TArray<UObject*> EventContexts; EventContexts.Add(GetAnimationPlaybackContext()); return EventContexts;
}

SkillAssetEditorSequenceTabSummoner::~SkillAssetEditorSequenceTabSummoner()
{
}
#undef LOCTEXT_NAMESPACE