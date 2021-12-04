// Fill out your copyright notice in the Description page of Project Settings.


#include "SKillEditorPreviewViewport.h"

#include "SkillEditorPreviewClient.h"
#include "SKillAssetPriveiwScene.h"
#include "SkillEditorViewportToolBar.h"
void SKillEditorPreviewViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

TSharedRef<SEditorViewport> SKillEditorPreviewViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SKillEditorPreviewViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SKillEditorPreviewViewport::OnFloatingButtonClicked()
{
	//do nothing!
	UE_LOG(LogTemp,Warning,L"Preview viewport Clicked!")
}

SKillEditorPreviewViewport::SKillEditorPreviewViewport()
{
}

SKillEditorPreviewViewport::~SKillEditorPreviewViewport()
{
}

TSharedRef<FEditorViewportClient> SKillEditorPreviewViewport::MakeEditorViewportClient()
{
	PreviewScene=MakeShareable(new SKillAssetPriveiwScene(SKillAssetPriveiwScene::ConstructionValues()));
	ViewportClient=MakeShareable(new SkillEditorPreviewClient
		(nullptr,PreviewScene.Get()));
	ViewportClient->ViewportType=LVT_Perspective;
	ViewportClient->bSetListenerPosition=false;
	return ViewportClient.ToSharedRef();
	
}

TSharedPtr<SWidget> SKillEditorPreviewViewport::MakeViewportToolbar()
{
	return SNew(SkillEditorViewportToolBar,SharedThis(this)).Cursor(EMouseCursor::Default);
	
}
