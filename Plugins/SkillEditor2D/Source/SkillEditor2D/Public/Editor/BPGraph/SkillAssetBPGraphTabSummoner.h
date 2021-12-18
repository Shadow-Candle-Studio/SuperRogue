// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SKAUEdGraph.h"
#include "WorkflowTabFactory.h"
#include "WorkflowUObjectDocuments.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillAssetBPGraphTabSummoner : public FDocumentTabFactoryForObjects<USKAUEdGraph>
{
public:
	~SkillAssetBPGraphTabSummoner() override;
	SkillAssetBPGraphTabSummoner(TSharedPtr<class FSkillAssetEditor> InEditor);
	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;
virtual const FSlateBrush* GetTabIcon(const FWorkflowTabSpawnInfo& Info) const override;
protected:virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	virtual TAttribute<FText> ConstructTabNameForObject(USKAUEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, USKAUEdGraph* DocumentID) const override;
	//virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, USKAUEdGraph* DocumentID) const override;
	
private:
	TWeakPtr<class FSkillAssetEditor> EditorPtr;
};
