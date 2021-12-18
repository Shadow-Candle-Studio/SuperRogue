// Fill out your copyright notice in the Description page of Project Settings.



#include "Editor/BPGraph/SkillAssetBPGraphTabSummoner.h"

#include "Editor/SkillAssetEditor.h"


#define LOCTEXT_NAMESPACE "SkillAssetBPGraphTabSummoner"

SkillAssetBPGraphTabSummoner::SkillAssetBPGraphTabSummoner(
	TSharedPtr<class FSkillAssetEditor> InEditorPtr):
FDocumentTabFactoryForObjects<USKAUEdGraph>(
	FSkillAssetEditor::GraphCanvasId,InEditorPtr),
EditorPtr(InEditorPtr)
{
	//InEditorPtr.Get()->GetSkillAsset()
	TabLabel=LOCTEXT("Events we added in this asset", "Events");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),
		"GraphEditor.EventGraph_16x");
	bIsSingleton=true;
	ViewMenuDescription = LOCTEXT("All of the events", "Events");
	ViewMenuTooltip = LOCTEXT("where we add our customized events",
		"Show all of the events");
	
}

void SkillAssetBPGraphTabSummoner::OnTabActivated(TSharedPtr<SDockTab> DockTab) const
{
	check(EditorPtr.IsValid())
	TSharedRef<SGraphEditor> GraphEditor=
		StaticCastSharedRef<SGraphEditor>(DockTab->GetContent());
	
}

void SkillAssetBPGraphTabSummoner::OnTabRefreshed(TSharedPtr<SDockTab> DockTab) const
{
	TSharedRef<SGraphEditor> GraphEditor=
		StaticCastSharedRef<SGraphEditor>
	(DockTab->GetContent());
	GraphEditor->NotifyGraphChanged();
	
	
}

TAttribute<FText> SkillAssetBPGraphTabSummoner::ConstructTabNameForObject(USKAUEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> SkillAssetBPGraphTabSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info,
	USKAUEdGraph* DocumentID) const
{
	check(DocumentID);
	SGraphEditor::FGraphEditorEvents InEvents;
	//InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(StateMachineEditor.Get(), &FStateMachineEditor::OnSelectedNodesChanged);
	//InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FStateMachineEditor::OnNodeDoubleClicked);
	//InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FStateMachineEditor::OnNodeTitleCommitted);
	FGraphAppearanceInfo AppearanceInfo;
	UE_LOG(LogTemp,Warning,L"CreateTabBodyForObject")
	//return OnCreateWidget.Execute(DocumentID);
	// Make title bar
	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("TestGraphLabel", "Events"))
		.TextStyle(FEditorStyle::Get(), TEXT("GraphBreadcrumbButtonText"))
		]
		];

	// Make full graph editor
	//const bool bGraphIsEditable = InGraph->bEditable;
	return SNew(SGraphEditor)
		//.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(DocumentID)
		.GraphEvents(InEvents);
}

// TSharedRef<SWidget> SkillAssetBPGraphTabSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
// {
// 	//UE_LOG(LogTemp,Warning,L"CreateTabBodyXXXXXXXXXXXXX")
// 	//return FDocumentTabFactoryForObjects<USKAUEdGraph>::CreateTabBody(Info);
// }

FText SkillAssetBPGraphTabSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillAssetEditorBPGraphTabToolTip","This BP Graph shows you all of the events you have added to this asset");
}




const FSlateBrush* SkillAssetBPGraphTabSummoner::GetTabIcon(const FWorkflowTabSpawnInfo& Info) const
{
	return FDocumentTabFactoryForObjects<USKAUEdGraph>::GetTabIcon(Info);
}

const FSlateBrush* SkillAssetBPGraphTabSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info,
	USKAUEdGraph* DocumentID) const
{
	return nullptr;
}


SkillAssetBPGraphTabSummoner::~SkillAssetBPGraphTabSummoner()
{
}


#undef LOCTEXT_NAMESPACE