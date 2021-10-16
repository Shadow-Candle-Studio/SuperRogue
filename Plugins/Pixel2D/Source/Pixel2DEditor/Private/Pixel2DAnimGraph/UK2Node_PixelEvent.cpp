// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UK2Node_PixelEvent.h"
#include "Pixel2DAnimBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Pixel2DAnimInstance.h"
#include "Pixel2DAnimGraphSchema.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintEventNodeSpawner.h"
#include "Pixel2DAnimGraphNode_AssetPlayerBase.h"
#include "Pixel2DAnimGraphNode_AssetSprite.h"

#define LOCTEXT_NAMESPACE "PixelEvent"

FText UK2Node_PixelEvent::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return CachedTitle;
}

bool UK2Node_PixelEvent::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return true;
}

void UK2Node_PixelEvent::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

			TArray<UPixel2DAnimGraphNode_AssetPlayerBase*> AssetPlayerNodes;
			FBlueprintEditorUtils::GetAllNodesOfClass(AnimBlueprint, AssetPlayerNodes);

			for (UPixel2DAnimGraphNode_Base* AssetNode : AssetPlayerNodes)
			{
				// Should always succeed

				if (UPixel2DAnimGraphNode_AssetSprite* AssetSprite = Cast<UPixel2DAnimGraphNode_AssetSprite>(AssetNode))
				{
					auto PostSpawnSetupLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, FName NotifyName)
					{
						UK2Node_PixelEvent* ActorRefNode = CastChecked<UK2Node_PixelEvent>(NewNode);
						ActorRefNode->EventReference.SetExternalMember(ActorRefNode->CustomFunctionName, UPixel2DAnimInstance::StaticClass());
						ActorRefNode->CachedTitle = FText::FromName(NotifyName);
					};

					for (int i = 0; i < AssetSprite->NotifyEventsNum(); i++)
					{
						FName NotifyName = AssetSprite->GetNotifyName(i);
						if (AssetSprite->IsNotifyEnabled(i))
						{
							UBlueprintNodeSpawner* NodeSpawner = UBlueprintEventNodeSpawner::Create(UK2Node_PixelEvent::StaticClass(), NotifyName);
							NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(PostSpawnSetupLambda, NotifyName);
							ActionRegistrar.AddBlueprintAction(AnimBlueprint, NodeSpawner);
						}
					}
				}
			}
		}
	}
}

bool UK2Node_PixelEvent::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
	return false;
}

#undef LOCTEXT_NAMESPACE
