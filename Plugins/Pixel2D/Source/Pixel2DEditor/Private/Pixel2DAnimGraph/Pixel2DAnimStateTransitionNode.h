// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Guid.h"
#include "Pixel2DAnimStateNodeBase.h"

#include "Pixel2DAnimStateTransitionNode.generated.h"

class UEdGraph;
class UEdGraphPin;

UCLASS(MinimalAPI, config = Editor)
class UPixel2DAnimStateTransitionNode : public UPixel2DAnimStateNodeBase
{
	GENERATED_UCLASS_BODY()

	// The transition logic graph for this transition (returning a boolean)
	UPROPERTY()
	class UEdGraph* BoundGraph;

	// The priority order of this transition. If multiple transitions out of a state go
	// true at the same time, the one with the smallest priority order will take precedent
	UPROPERTY(EditAnywhere, Category = Transition)
	int32 PriorityOrder;

	// The duration to cross-fade for
	UPROPERTY(EditAnywhere, Config, Category = Transition)
	float CrossfadeDuration;

	// Try setting the rule automatically based on most relevant player node's remaining time and the CrossfadeDuration of the transition, ignoring the internal time
	UPROPERTY(EditAnywhere, Category = Transition)
	bool bAutomaticRuleBasedOnSequencePlayerInState;

	/** This transition can go both directions */
	UPROPERTY(EditAnywhere, Category = Transition)
	bool Bidirectional;

	/** The rules for this transition may be shared with other transition nodes */
	UPROPERTY()
	bool bSharedRules;

	/** The cross-fade settings of this node may be shared */
	UPROPERTY()
	bool bSharedCrossfade;

	/** What we call this transition if we are shared ('Transition X to Y' can't be used since its used in multiple places) */
	UPROPERTY()
	FString	SharedRulesName;

	/** Shared rules guid useful when copying between different state machines */
	UPROPERTY()
	FGuid SharedRulesGuid;

	/** Color we draw in the editor as if we are shared */
	UPROPERTY()
	FLinearColor SharedColor;

	UPROPERTY()
	FString SharedCrossfadeName;

	UPROPERTY()
	FGuid SharedCrossfadeGuid;

	UPROPERTY()
	int32 SharedCrossfadeIdx;

	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual bool CanDuplicateNode() const override { return true; }
	virtual void PrepareForCopying() override;
	virtual void PostPasteNode() override;
	virtual void PostPlacedNewNode() override;
	virtual void DestroyNode() override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	//~ End UEdGraphNode Interface

	//~ Begin UAnimStateNodeBase Interface
	virtual UEdGraph* GetBoundGraph() const override { return BoundGraph; }
	virtual void ClearBoundGraph() override { BoundGraph = nullptr; }
	virtual UEdGraphPin* GetInputPin() const override { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const override { return Pins[1]; }
	//~ End UAnimStateNodeBase Interface

	// @return the name of this state
	PIXEL2DEDITOR_API FString GetStateName() const override;

	PIXEL2DEDITOR_API UPixel2DAnimStateNodeBase* GetPreviousState() const;
	PIXEL2DEDITOR_API UPixel2DAnimStateNodeBase* GetNextState() const;
	PIXEL2DEDITOR_API void CreateConnections(UPixel2DAnimStateNodeBase* PreviousState, UPixel2DAnimStateNodeBase* NextState);

	PIXEL2DEDITOR_API bool IsBoundGraphShared();

	PIXEL2DEDITOR_API void MakeRulesShareable(FString ShareName);
	PIXEL2DEDITOR_API void MakeCrossfadeShareable(FString ShareName);

	PIXEL2DEDITOR_API void UnshareRules();
	PIXEL2DEDITOR_API void UnshareCrossade();

	PIXEL2DEDITOR_API void UseSharedRules(const UPixel2DAnimStateTransitionNode* Node);
	PIXEL2DEDITOR_API void UseSharedCrossfade(const UPixel2DAnimStateTransitionNode* Node);

	void CopyCrossfadeSettings(const UPixel2DAnimStateTransitionNode* SrcNode);
	void PropagateCrossfadeSettings();

	PIXEL2DEDITOR_API bool IsReverseTrans(const UPixel2DAnimStateNodeBase* Node);
protected:
	void CreateBoundGraph();
};
