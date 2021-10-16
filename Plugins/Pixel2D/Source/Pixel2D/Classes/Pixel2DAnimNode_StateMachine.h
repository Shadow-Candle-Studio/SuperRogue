// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimStateMachineTypes.h"
#include"Pixel2DAnimNode_Base.h"
#include "Pixel2DAnimNode_StateMachine.generated.h"

USTRUCT()
struct FPixel2DAnimationPotentialTransition
{
	GENERATED_USTRUCT_BODY()

	int32 TargetState;

	const FBakedStateExitTransition* TransitionRule;

	TArray<int32, TInlineAllocator<3>> SourceTransitionIndices;

public:
	FPixel2DAnimationPotentialTransition();
	bool IsValid() const;
	void Clear();
};

// State machine node
USTRUCT()
struct PIXEL2D_API FPixel2DAnimNode_StateMachine : public FPixel2DAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

public:
	// Index into the BakedStateMachines array in the owning UAnimBlueprintGeneratedClass
	UPROPERTY()
	int32 StateMachineIndexInClass;

	// The maximum number of transitions that can be taken by this machine 'simultaneously' in a single frame
	UPROPERTY(EditAnywhere, Category = Settings)
	int32 MaxTransitionsPerFrame;

	// Skip transition from entry state on first update?
	// default is true, we throw away transition data on first update
	UPROPERTY(EditAnywhere, Category = Settings)
	bool bSkipFirstUpdateTransition;

	// Reinitialize the state machine if we have become relevant to the graph
	// after not being ticked on the previous frame(s)
	UPROPERTY(EditAnywhere, Category = Settings)
	bool bReinitializeOnBecomingRelevant;

public:

	float GetCurrentStateElapsedTime() const
	{
		return ElapsedTime;
	}

protected:
	// The state machine description this is an instance of
	const FBakedAnimationStateMachine* PRIVATE_MachineDescription;

	// The current state within the state machine
	UPROPERTY()
	int32 CurrentState;

	// Elapsed time since entering the current state
	UPROPERTY()
	float ElapsedTime;

	// The set of states in this state machine
	TArray<FPixel2DPoseLink> StatePoseLinks;

private:
	// true if it is the first update.
	bool bFirstUpdate;

public:
	FPixel2DAnimNode_StateMachine()
		: StateMachineIndexInClass(0)
		, MaxTransitionsPerFrame(3)
		, bSkipFirstUpdateTransition(true)
		, bReinitializeOnBecomingRelevant(true)
		, PRIVATE_MachineDescription(NULL)
		, CurrentState(INDEX_NONE)
		, ElapsedTime(0.0f)
		, bFirstUpdate(true)
	{
	}

	// FPixel2DAnimNode_Base interface
	virtual void Initialize_AnyThread(const FPixel2DAnimationInitializeContext& Context) override;
	virtual void Update_AnyThread(const FPixel2DAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPixel2DPoseContext& Output) override;
	// End of FPixel2DAnimNode_Base interface

	const FBakedAnimationState& GetStateInfo(int32 StateIndex) const;
	const FAnimationTransitionBetweenStates& GetTransitionInfo(int32 TransIndex) const;

	bool IsValidTransitionIndex(int32 TransitionIndex) const;

//    /** Cache the internal machine description */
	void CacheMachineDescription(UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintClass);

protected:
	// Tries to get the instance information for the state machine
	const FBakedAnimationStateMachine* GetMachineDescription() const;

	void SetState(const FPixel2DAnimationBaseContext& Context, int32 NewStateIndex);
	void SetStateInternal(int32 NewStateIndex);

	const FBakedAnimationState& GetStateInfo() const;
	const int32 GetStateIndex(const FBakedAnimationState& StateInfo) const;

	// finds the highest priority valid transition, information pass via the OutPotentialTransition variable.
	// OutVisitedStateIndices will let you know what states were checked, but is also used to make sure we don't get stuck in an infinite loop or recheck states
	bool FindValidTransition(const FPixel2DAnimationUpdateContext& Context,
		const FBakedAnimationState& StateInfo,
		/*OUT*/ FPixel2DAnimationPotentialTransition& OutPotentialTransition,
		/*OUT*/ TArray<int32, TInlineAllocator<4>>& OutVisitedStateIndices);
};
