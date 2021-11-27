// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class SKILLEDITOR2D_API FSkillEditorViewPortRenderingClient: public FViewportClient
{
public:
	virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;
	FSkillEditorViewPortRenderingClient();
	~FSkillEditorViewPortRenderingClient();
	TSharedPtr<UTextureRenderTarget2D> RenderTarget;
};
