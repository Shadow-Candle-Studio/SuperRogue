// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetPropertyTabSummoner.h"

#include "SkillAssetEditor.h"
#define LOCTEXT_NAMESPACE "SkillAssetPropertyTabSummoner"

SkillAssetPropertyTabSummoner::SkillAssetPropertyTabSummoner(TSharedPtr<FSkillAssetEditor> InEditorPtr):
FWorkflowTabFactory(FSkillAssetEditor::PropertiesPanelTabID, InEditorPtr),EditorPtr(InEditorPtr)
{
	TabLabel=LOCTEXT("Details with preview", "DetailsOfSkillAsset");
	TabIcon=FSlateIcon(FEditorStyle::GetStyleSetName(),"Kismet.Tabs.Components");
	bIsSingleton=true;
	ViewMenuDescription = LOCTEXT("Details with preview", "DetailsOfSkillAsset");
	ViewMenuTooltip = LOCTEXT("Details with preview_ToolTip", "Show the details view");

	
	FSkillAssetEditor* Editor=EditorPtr.Pin().Get();
	
	// Cache some values that will be used for our details view arguments
	const bool bIsUpdatable = false;
	const bool bAllowFavorites = true;
	const bool bIsLockable = false;
	
	// Retrieve the property editor module and assign properties to DetailsView
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(bIsUpdatable, bIsLockable, true, FDetailsViewArgs::ObjectsUseNameArea, false);
	TabDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	
	// Set the asset we are editing in the details view
	if (TabDetailsView.IsValid())
	{
		TabDetailsView->SetObject((UObject*)Editor->GetSkillAsset());
	}
}

TSharedRef<SWidget> SkillAssetPropertyTabSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SNew(SVerticalBox)
	+SVerticalBox::Slot()
	.FillHeight(1.0f)
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
	TabDetailsView.ToSharedRef()
	];
}

FText SkillAssetPropertyTabSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillAssetEditorPropertiesTabToolTip","This property panel shows you details of properties of the Skill Asset");
}


SkillAssetPropertyTabSummoner::~SkillAssetPropertyTabSummoner()
{
}
#undef LOCTEXT_NAMESPACE