// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

/*=============================================================================
Pixel2DAnimStateNodeBase.cpp
=============================================================================*/

#include "Pixel2DAnimStateNodeBase.h"
#include "UObject/FrameworkObjectVersion.h"
#include "Pixel2DAnimBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Pixel2DAnimStateMachineGraph.h"
#include "Pixel2DAnimStateMachineSchema.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/KismetEditorUtilities.h"

/////////////////////////////////////////////////////
// FPixel2DAnimStateNodeNameValidator

class FPixel2DAnimStateNodeNameValidator : public FStringSetNameValidator
{
public:
	FPixel2DAnimStateNodeNameValidator(const UPixel2DAnimStateNodeBase* InStateNode)
		: FStringSetNameValidator(FString())
	{
		TArray<UPixel2DAnimStateNodeBase*> Nodes;
		UPixel2DAnimStateMachineGraph* StateMachine = CastChecked<UPixel2DAnimStateMachineGraph>(InStateNode->GetOuter());

		StateMachine->GetNodesOfClass<UPixel2DAnimStateNodeBase>(Nodes);
		for (auto NodeIt = Nodes.CreateIterator(); NodeIt; ++NodeIt)
		{
			auto Node = *NodeIt;
			if (Node != InStateNode)
			{
				Names.Add(Node->GetStateName());
			}
		}
	}
};

/////////////////////////////////////////////////////
// UPixel2DAnimStateNodeBase

UPixel2DAnimStateNodeBase::UPixel2DAnimStateNodeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPixel2DAnimStateNodeBase::PostPasteNode()
{
	Super::PostPasteNode();

	if (UEdGraph* BoundGraph = GetBoundGraph())
	{
		// Add the new graph as a child of our parent graph
		UEdGraph* ParentGraph = GetGraph();

		if (ParentGraph->SubGraphs.Find(BoundGraph) == INDEX_NONE)
		{
			ParentGraph->SubGraphs.Add(BoundGraph);
		}

		// Restore transactional flag that is lost during copy/paste process
		BoundGraph->SetFlags(RF_Transactional);

		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(ParentGraph);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	}
}

UObject* UPixel2DAnimStateNodeBase::GetJumpTargetForDoubleClick() const
{
	return GetBoundGraph();
}

bool UPixel2DAnimStateNodeBase::CanJumpToDefinition() const
{
	return GetJumpTargetForDoubleClick() != nullptr;
}

void UPixel2DAnimStateNodeBase::JumpToDefinition() const
{
	if (UObject* HyperlinkTarget = GetJumpTargetForDoubleClick())
	{
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(HyperlinkTarget);
	}
}

bool UPixel2DAnimStateNodeBase::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UPixel2DAnimStateMachineSchema::StaticClass());
}

void UPixel2DAnimStateNodeBase::OnRenameNode(const FString& NewName)
{
	FBlueprintEditorUtils::RenameGraph(GetBoundGraph(), NewName);
}

TSharedPtr<class INameValidatorInterface> UPixel2DAnimStateNodeBase::MakeNameValidator() const
{
	return MakeShareable(new FPixel2DAnimStateNodeNameValidator(this));
}

void UPixel2DAnimStateNodeBase::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar.UsingCustomVersion(FFrameworkObjectVersion::GUID);
}

void UPixel2DAnimStateNodeBase::PostLoad()
{
	Super::PostLoad();

	const int32 CustomVersion = GetLinkerCustomVersion(FFrameworkObjectVersion::GUID);

	if (CustomVersion < FFrameworkObjectVersion::FixNonTransactionalPins)
	{
		int32 BrokenPinCount = 0;
		for (UEdGraphPin_Deprecated* Pin : DeprecatedPins)
		{
			if (!Pin->HasAnyFlags(RF_Transactional))
			{
				++BrokenPinCount;
				Pin->SetFlags(Pin->GetFlags() | RF_Transactional);
			}
		}

		if (BrokenPinCount > 0)
		{
			UE_LOG(LogAnimation, Log, TEXT("Fixed %d non-transactional pins in %s"), BrokenPinCount, *GetName());
		}
	}
}

UPixel2DAnimBlueprint* UPixel2DAnimStateNodeBase::GetSpriteAnimBlueprint() const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNode(this);
	return CastChecked<UPixel2DAnimBlueprint>(Blueprint);
}