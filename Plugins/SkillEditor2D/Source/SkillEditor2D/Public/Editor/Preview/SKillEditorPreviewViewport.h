// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"



/**
 * 
 */
class SKILLEDITOR2D_API SKillEditorPreviewViewport: public SEditorViewport ,public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SKillEditorPreviewViewport){}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;
	SKillEditorPreviewViewport();
	~SKillEditorPreviewViewport();
	

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
private:
	TSharedPtr<class FEditorViewportClient> ViewportClient;
	TSharedPtr<class SKillAssetPriveiwScene> PreviewScene;
};
