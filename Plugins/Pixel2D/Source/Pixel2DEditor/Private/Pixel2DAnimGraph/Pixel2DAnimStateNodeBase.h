// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "Pixel2DAnimStateNodeBase.generated.h"

class INameValidatorInterface;
class UEdGraph;
class UEdGraphPin;
class UEdGraphSchema;
class UPixel2DAnimBlueprint;

UCLASS(MinimalAPI, abstract)
class UPixel2DAnimStateNodeBase : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

	virtual void Serialize(FArchive& Ar) override;
	virtual void PostLoad() override;

	// UEdGraphNode interface
	virtual void PostPasteNode() override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	// End of UEdGraphNode interface

	// @return the input pin for this state
	PIXEL2DEDITOR_API virtual UEdGraphPin* GetInputPin() const { return NULL; }

	// @return the output pin for this state
	PIXEL2DEDITOR_API virtual UEdGraphPin* GetOutputPin() const { return NULL; }

	// @return the name of this state
	PIXEL2DEDITOR_API virtual FString GetStateName() const { return TEXT("BaseState"); }

	// Populates the OutTransitions array with a list of transition nodes connected to this state
	PIXEL2DEDITOR_API virtual void GetTransitionList(TArray<class UPixel2DAnimStateTransitionNode*>& OutTransitions, bool bWantSortedList = false) { }

	virtual UEdGraph* GetBoundGraph() const { return NULL; }
	virtual void ClearBoundGraph() {}

	PIXEL2DEDITOR_API UPixel2DAnimBlueprint* GetSpriteAnimBlueprint() const;

protected:
	// Name used as a seed when pasting nodes
	virtual FString GetDesiredNewNodeName() const { return TEXT("State"); }

};
