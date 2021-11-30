// Fill out your copyright notice in the Description page of Project Settings.


#include "FSkillEditorViewPortRenderingClient.h"

#include "CanvasItem.h"
#include "FSkillEditorShaderRenderer.h"
#include "SkillEditorRenderer.h"
#include "Engine/TextureRenderTarget2D.h"


void FSkillEditorViewPortRenderingClient::Draw(FViewport* Viewport, FCanvas* Canvas)
{
	Canvas->Clear(FLinearColor::Black);
	if (RenderTarget)
	{
		FSkillEditorShaderRenderer render;
		render.Render(RenderTarget.Get());
		
		FCanvasTileItem Tile(FVector2D::ZeroVector, RenderTarget->Resource, Viewport->GetSizeXY(), FLinearColor(1,1,1,1));
		Canvas->DrawItem(Tile);
	}
}

FSkillEditorViewPortRenderingClient::FSkillEditorViewPortRenderingClient()
{
	RenderTarget =MakeShareable(NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient));
	RenderTarget->RenderTargetFormat = RTF_RGBA8;
	RenderTarget->InitAutoFormat(1024, 1024);
	RenderTarget->ClearColor = FLinearColor::Blue;
	RenderTarget->UpdateResource();
}

FSkillEditorViewPortRenderingClient::~FSkillEditorViewPortRenderingClient()
{
	RenderTarget->ReleaseResource();
}
