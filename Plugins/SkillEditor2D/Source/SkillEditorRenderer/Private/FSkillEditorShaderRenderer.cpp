// Fill out your copyright notice in the Description page of Project Settings.


#include "FSkillEditorShaderRenderer.h"

#include "SkillEditorRenderer.h"
#include "Engine/TextureRenderTarget2D.h"


#include "Classes/Engine/TextureRenderTarget2D.h"
#include "Classes/Engine/World.h"
#include "Public/GlobalShader.h"
#include "Public/PipelineStateCache.h"
#include "Public/RHIStaticStates.h"
#include "Public/SceneUtils.h"
#include "Public/SceneInterface.h"
#include "Public/ShaderParameterUtils.h"
#include "Engine/Public/PlatformFeatures.h"




FSkillEditorShaderRenderer_Base::FSkillEditorShaderRenderer_Base()
{
}

FSkillEditorShaderRenderer_Base::~FSkillEditorShaderRenderer_Base()
{
	;
}
#define GETSAFERHISHADER_PIXEL(Shader) ((Shader) ? (Shader).GetPixelShader() : nullptr)
#define GETSAFERHISHADER_VERTEX(Shader) ((Shader) ? (Shader).GetVertexShader() : nullptr)

IMPLEMENT_SHADER_TYPE(,
	FSkillEditorShaderRendererVS,
	TEXT("/Plugins/SkillEditor2D/ShaderSample.usf"),
	TEXT("MainVS"),
	SF_Vertex)
IMPLEMENT_SHADER_TYPE(,
	FSkillEditorShaderRendererPS,
	TEXT("/Plugins/SkillEditor2D/ShaderSample.usf"),
	TEXT("MainPS"),
	SF_Pixel)

static void DrawSkillEditorShaderRenderTarget_RenderThread(
FRHICommandListImmediate& RHICmdList,
FTextureRenderTargetResource* OutputRenderTargetResource,
TGlobalResource
		 <FSkillEditorShaderRendererVertexBuffer>& G_SFSkillEditorShaderRendererVertexBuffer,
TGlobalResource 
		<FSkillEditorShaderRendererVertexDeclaration>& G_FSkillEditorShaderRendererVertexDeclaration,
TGlobalResource
		<FSkillEditorShaderRendererIndexBuffer>& G_FSkillEditorShaderRendererIndexBuffer
)
{
	auto& mod=
	FModuleManager::GetModuleChecked<FSkillEditorRenderer>(TEXT("SkillEditorRenderer"));
	
	if(IsInRenderingThread())
	{
		FRHIRenderPassInfo RPInfo(OutputRenderTargetResource->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_Store, OutputRenderTargetResource->TextureRHI);
		RHICmdList.BeginRenderPass(RPInfo, TEXT("SimpleRendererShaderPass"));
		{
			// Get shaders.
			//change a little bit of the code
		
			FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
			TShaderMapRef<FSkillEditorShaderRendererVS> VertexShader(GlobalShaderMap);
			TShaderMapRef<FSkillEditorShaderRendererPS> PixelShader(GlobalShaderMap);

			// Set the graphic pipeline state.
			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
			GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;
			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = G_FSkillEditorShaderRendererVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

			// Update viewport.
			RHICmdList.SetViewport(
				0, 0, 0.f,
				OutputRenderTargetResource->GetSizeX(), OutputRenderTargetResource->GetSizeY(), 1.f);

			// Set the vertextBuffer.
			RHICmdList.SetStreamSource(0,
				G_SFSkillEditorShaderRendererVertexBuffer.
				VertexBufferRHI,
				0);

			RHICmdList.DrawIndexedPrimitive(

			G_FSkillEditorShaderRendererIndexBuffer.
				IndexBufferRHI,
				/*BaseVertexIndex=*/ 0,
				/*MinIndex=*/ 0,
				/*NumVertices=*/ 3,
				/*StartIndex=*/ 0,
				/*NumPrimitives=*/ 1,
				/*NumInstances=*/ 1);
		}
		RHICmdList.EndRenderPass();
	}
	else
		;
	
}
void FSkillEditorShaderRenderer::Render(UTextureRenderTarget2D* RenderTarget)
{
	if(IsInGameThread())
	{
		if(!RenderTarget)
			return;

		FTextureRenderTargetResource*
			TextureRenderTargetResource = RenderTarget->
		GameThread_GetRenderTargetResource();

		ENQUEUE_RENDER_COMMAND(CaptureCommand)(
	[TextureRenderTargetResource,this](FRHICommandListImmediate& RHICmdList)
	 {
		DrawSkillEditorShaderRenderTarget_RenderThread(RHICmdList,
			TextureRenderTargetResource,this->G_SFSkillEditorShaderRendererVertexBuffer,
			this->G_FSkillEditorShaderRendererVertexDeclaration,this->G_FSkillEditorShaderRendererIndexBuffer);
	 }


		);
	}

	
}

void FSkillEditorShaderRenderer::ReleaseAllResource()
{
	//Resources::G_FSkillEditorShaderRendererIndexBuffer.ReleaseResource();
	//FSkillEditorShaderRenderer::G_SFSkillEditorShaderRendererVertexBuffer->ReleaseResource();
	//FSkillEditorShaderRenderer::G_FSkillEditorShaderRendererVertexDeclaration->ReleaseResource();
}
