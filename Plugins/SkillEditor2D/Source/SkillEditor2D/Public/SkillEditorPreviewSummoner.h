// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowTabFactory.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillEditorPreviewSummoner : FWorkflowTabFactory
{
public:
	SkillEditorPreviewSummoner(TSharedPtr<class FSkillAssetEditor> InSKillEditorPtr);
	virtual TSharedRef<SWidget>
	CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	
	~SkillEditorPreviewSummoner();

protected:
	TWeakPtr<class FSkillAssetEditor> SKillEditor;
};
