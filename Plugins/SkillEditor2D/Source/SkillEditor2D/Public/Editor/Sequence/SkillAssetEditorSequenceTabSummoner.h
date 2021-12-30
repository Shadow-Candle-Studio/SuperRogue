// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WorkflowTabFactory.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillAssetEditorSequenceTabSummoner : public FWorkflowTabFactory
{
public:
	SkillAssetEditorSequenceTabSummoner(TSharedPtr<class FSkillAssetEditor> InEditorPtr);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

	void getNewSequencer()
	{
		;
	}
	void OnGetAnimationAddMenuContent( FMenuBuilder& /*MenuBuilder*/, TSharedRef<class ISequencer>);
	void OnBuildCustomContextMenuForGuid(FMenuBuilder&, FGuid);
	UObject* GetAnimationPlaybackContext() const;
	TArray<UObject*>GetAnimationEventContexts() const;
	~SkillAssetEditorSequenceTabSummoner();
	TSharedPtr<IToolkitHost> ToolkitHost;
	TSharedPtr<class ISequencer> TabSequencer;
private:
	TWeakPtr<class FSkillAssetEditor> EditorPtr;
};
