// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SKILLEDITOR2D_API SkillEditorPreviewClient:public FEditorViewportClient
{
public:
	virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual UWorld* GetWorld() const override;
	SkillEditorPreviewClient(FEditorModeTools* InModeTools, FPreviewScene* InPreviewScene = nullptr, const TWeakPtr<SEditorViewport>& InEditorViewportWidget = nullptr);
	~SkillEditorPreviewClient();
};
