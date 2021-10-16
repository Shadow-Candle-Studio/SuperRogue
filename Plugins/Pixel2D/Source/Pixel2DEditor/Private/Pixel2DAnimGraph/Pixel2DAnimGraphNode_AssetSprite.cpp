// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimGraphNode_AssetSprite.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorActions.h"
#include "ToolMenus/Public/ToolMenu.h"
#include "Kismet2/CompilerResultsLog.h"
#include "PaperFlipbook.h"
#include "K2Node_Event.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintActionDatabase.h"
#include "BlueprintEventNodeSpawner.h"
#include "Pixel2DAnimNotify.h"

/////////////////////////////////////////////////////
// UPixel2DAnimGraphNode_AssetSprite

#define LOCTEXT_NAMESPACE "A3Nodes"

UPixel2DAnimGraphNode_AssetSprite::UPixel2DAnimGraphNode_AssetSprite(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NotifyProxy = NewObject<UPixel2DAnimNotify>();
	NotifyProxy->NotifyArray = &(Node.NotifyEvents);
}

void UPixel2DAnimGraphNode_AssetSprite::InitNotifyProxy()
{
	if (NotifyProxy == nullptr)
	{
		NotifyProxy = NewObject<UPixel2DAnimNotify>();
	}
	NotifyProxy->Init(Node.NotifyEvents);
}

FText UPixel2DAnimGraphNode_AssetSprite::GetTooltipText() const
{
	// FText::Format() is slow, so we utilize the cached list title
	return GetNodeTitle(ENodeTitleType::ListView);
}

FText UPixel2DAnimGraphNode_AssetSprite::GetNodeTitleForFlipbook(ENodeTitleType::Type TitleType, UPaperFlipbook* Flipbook) const
{
	const FText FlipbookName = FText::FromString(Flipbook->GetName());

	if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("Flipbook"), FlipbookName);

		// FText::Format() is slow, so we cache this to save on performance
		CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AssetSpriteListTitle", "Flipbook '{FlipbookName}'"), Args), this);
	}
	else
	{
		FFormatNamedArguments TitleArgs;
		TitleArgs.Add(TEXT("FlipbookName"), FlipbookName);
		FText Title = FText::Format(LOCTEXT("AssetSpriteFullTitle", "{FlipbookName}\nFlipbook"), TitleArgs);

		// FText::Format() is slow, so we cache this to save on performance
		CachedNodeTitles.SetCachedTitle(TitleType, Title, this);
	}

	return CachedNodeTitles[TitleType];
}

FText UPixel2DAnimGraphNode_AssetSprite::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Node.AssetFlipbook == nullptr)
	{
			return LOCTEXT("SpriteAsset_Title", "Unknown Flipbook");
	}
	else //if (!CachedNodeTitles.IsTitleCached(TitleType, this))
	{
		return GetNodeTitleForFlipbook(TitleType, Node.AssetFlipbook);
	}
}

// REVISIT
void UPixel2DAnimGraphNode_AssetSprite::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// First cache the available functions for getters
	UClass* ActionKey = GetClass();
	const UPixel2DAnimBlueprint* AnimBlueprint = Cast<UPixel2DAnimBlueprint>(ActionRegistrar.GetActionKeyFilter());
	if (AnimBlueprint && ActionRegistrar.IsOpenForRegistration(AnimBlueprint))
	{
		UClass* BPClass = *AnimBlueprint->ParentClass;
		while (BPClass && !BPClass->HasAnyClassFlags(CLASS_Native))
		{
			BPClass = BPClass->GetSuperClass();
		}

		if (BPClass)
		{

			auto UiSpecOverride = [](const FBlueprintActionContext& /*Context*/, const IBlueprintNodeBinder::FBindingSet& Bindings, FBlueprintActionUiSpec* UiSpecOut, FText Title)
			{
				UiSpecOut->MenuName = Title;
			};

			auto PostSpawnSetupLambda = [](UEdGraphNode* NewNode, bool /*bIsTemplateNode*/)
			{
				UK2Node_Event* ActorRefNode = CastChecked<UK2Node_Event>(NewNode);
			};

			UBlueprintNodeSpawner* NodeSpawner    = UBlueprintEventNodeSpawner::Create(UK2Node_Event::StaticClass(), FName(TEXT("ThisIsMyTestFName")));
			NodeSpawner->CustomizeNodeDelegate    = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(PostSpawnSetupLambda);
		}
	}
}

void UPixel2DAnimGraphNode_AssetSprite::GetNodeContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	if (!Context->bIsDebugging)
	{
		FToolMenuSection& Section = Menu->AddSection("Pixel2DAnimGraphNodeAssetSprite", NSLOCTEXT("A3Nodes", "AssetSpriteHeading", "Asset Sprite"));
		{
			Section.AddMenuEntry(FGraphEditorCommands::Get().OpenRelatedAsset);
		}
	}
}

void UPixel2DAnimGraphNode_AssetSprite::SetAnimationAsset(UPaperFlipbook* Asset)
{
	{
		Node.AssetFlipbook = Asset;
	}
}

EAnimAssetHandlerType UPixel2DAnimGraphNode_AssetSprite::SupportsAssetClass(const UClass* AssetClass) const
{
	if (AssetClass->IsChildOf(UPaperFlipbook::StaticClass()))
	{
		return EAnimAssetHandlerType::PrimaryHandler;
	}
	else
	{
		return EAnimAssetHandlerType::NotSupported;
	}
}

FName UPixel2DAnimGraphNode_AssetSprite::GetNotifyName(int32 NotifyIndex)
{
	return Node.GetNotifyName(NotifyIndex);
}

UPixel2DAnimNotify * UPixel2DAnimGraphNode_AssetSprite::GetNotify()
{
	// REVISIT
	//if (NotifyProxy == nullptr)
	//{
		InitNotifyProxy();
	//}
	return NotifyProxy;
}

bool UPixel2DAnimGraphNode_AssetSprite::IsNotifyEnabled(int32 NotifyIndex)
{
	return Node.NotifyEvents[NotifyIndex].bEnabled;
}

TArray<FName> UPixel2DAnimGraphNode_AssetSprite::NotifyEventsNames()
{
	TArray<FName> RetVal;

	for (int i = 0; i < NotifyEventsNum(); i++)
	{
		RetVal.Push(GetNotifyName(i));
	}

	return RetVal;
}

#undef LOCTEXT_NAMESPACE
