// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"

struct FColorVertex {
	FVector2D Position;
	FVector4 Color;
};
/**
 * 
 */
//base shader class
class SKILLEDITORRENDERER_API FSkillEditorShaderRenderer_Base : public FGlobalShader
{
public:
	FSkillEditorShaderRenderer_Base();
	FSkillEditorShaderRenderer_Base(
		const ShaderMetaType::
		CompiledShaderInitializerType&
		Initializer):FGlobalShader(Initializer)
	{}
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM4_REMOVED);
	}
	~FSkillEditorShaderRenderer_Base();
};
// vertex shader class
class SKILLEDITORRENDERER_API FSkillEditorShaderRendererVS : public FSkillEditorShaderRenderer_Base
{
	DECLARE_SHADER_TYPE(FSkillEditorShaderRendererVS, Global);
	
public:
	FSkillEditorShaderRendererVS(){}
	FSkillEditorShaderRendererVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FSkillEditorShaderRenderer_Base(Initializer)
	{
	}
};
//pixel shader class
class SKILLEDITORRENDERER_API FSkillEditorShaderRendererPS : public FSkillEditorShaderRenderer_Base
{
	DECLARE_SHADER_TYPE(FSkillEditorShaderRendererPS, Global);
	
public:
	FSkillEditorShaderRendererPS(){}
	FSkillEditorShaderRendererPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FSkillEditorShaderRenderer_Base(Initializer)
	{
	}
};


// vertex buffer class
class SKILLEDITORRENDERER_API FSkillEditorShaderRendererVertexBuffer : public FVertexBuffer
{
public:
	void InitRHI() override
	{
		TResourceArray<FColorVertex, VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(3);

		Vertices[0].Position = FVector2D(0, 0.75);
		Vertices[0].Color = FVector4(1, 0, 0, 1);

		Vertices[1].Position = FVector2D(0.75, -0.75);
		Vertices[1].Color = FVector4(0, 1, 0, 1);

		Vertices[2].Position = FVector2D(-0.75, -0.75);
		Vertices[2].Color = FVector4(0, 0, 1, 1);
    
		FRHIResourceCreateInfo CreateInfo(&Vertices);
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.GetResourceDataSize(), BUF_Static, CreateInfo);
	}
};



//vertex declaration
class SKILLEDITORRENDERER_API FSkillEditorShaderRendererVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FColorVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FColorVertex, Position), VET_Float2, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FColorVertex, Color), VET_Float4, 1, Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}
	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};



//index buffer
class SKILLEDITORRENDERER_API FSkillEditorShaderRendererIndexBuffer : public FIndexBuffer
{
public:
	void InitRHI() override
	{
		const uint16 Indices[] = { 0, 1, 2 };

		TResourceArray<uint16, INDEXBUFFER_ALIGNMENT> IndexBuffer;
		uint32 NumIndices = UE_ARRAY_COUNT(Indices);
		IndexBuffer.AddUninitialized(NumIndices);
		FMemory::Memcpy(IndexBuffer.GetData(), Indices, NumIndices * sizeof(uint16));

		FRHIResourceCreateInfo CreateInfo(&IndexBuffer);
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16), IndexBuffer.GetResourceDataSize(), BUF_Static, CreateInfo);
	}
};




class SKILLEDITORRENDERER_API FSkillEditorShaderRenderer
{
public:
	FSkillEditorShaderRenderer(){};
	void Render(class UTextureRenderTarget2D* RenderTarget);
	static void ReleaseAllResource();
	TGlobalResource
		 <FSkillEditorShaderRendererVertexBuffer>
	G_SFSkillEditorShaderRendererVertexBuffer;
	TGlobalResource
		<FSkillEditorShaderRendererVertexDeclaration>
		G_FSkillEditorShaderRendererVertexDeclaration;
	
	TGlobalResource
		<FSkillEditorShaderRendererIndexBuffer>
		G_FSkillEditorShaderRendererIndexBuffer;
	
	
};
