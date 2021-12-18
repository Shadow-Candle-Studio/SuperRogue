// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorWindowStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"



TSharedPtr<FSlateStyleSet> SkillEditorWindowStyle::StyleInstance=NULL;

void SkillEditorWindowStyle::Initialize()
{
	if(!StyleInstance.IsValid())
	{
		StyleInstance=Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void SkillEditorWindowStyle::ShutDown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}



FName SkillEditorWindowStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SkillEditorWindowStyle"));
	return StyleSetName;
}
#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<FSlateStyleSet> SkillEditorWindowStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("SkillEditorWindowStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SkillEditor2D")->GetBaseDir() / TEXT("Resources"));

	Style->Set("SkillEditor2D.OpenPluginWindow", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));

	return Style;
}
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void SkillEditorWindowStyle::Reloadtextures()
{
	if(FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& SkillEditorWindowStyle::Get()
{
	return *StyleInstance;
}
SkillEditorWindowStyle::SkillEditorWindowStyle()
{
}

SkillEditorWindowStyle::~SkillEditorWindowStyle()
{
}
