// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNode_StateMachine.h"
#include "Pixel2DAnimInstanceProxy.h"
#include "Pixel2DAnimNode_TransitionResult.h"

/////////////////////////////////////////////////////
// FPixel2DAnimationPotentialTransition

FPixel2DAnimationPotentialTransition::FPixel2DAnimationPotentialTransition()
	: TargetState(INDEX_NONE)
	, TransitionRule(NULL)
{
}

bool FPixel2DAnimationPotentialTransition::IsValid() const
{
	return (TargetState != INDEX_NONE) && (TransitionRule != NULL) && (TransitionRule->TransitionIndex != INDEX_NONE);
}

void FPixel2DAnimationPotentialTransition::Clear()
{
	TargetState = INDEX_NONE;
	TransitionRule = NULL;
	SourceTransitionIndices.Reset();
}

/////////////////////////////////////////////////////
// FPixel2DAnimNode_StateMachine

// Tries to get the instance information for the state machine
const FBakedAnimationStateMachine* FPixel2DAnimNode_StateMachine::GetMachineDescription() const
{
	if (PRIVATE_MachineDescription != nullptr)
	{
		return PRIVATE_MachineDescription;
	}
	else
	{
		UE_LOG(LogAnimation, Warning, TEXT("FAnimNode_StateMachine: Bad machine ptr"));
		return nullptr;
	}
}

void FPixel2DAnimNode_StateMachine::Initialize_AnyThread(const FPixel2DAnimationInitializeContext& Context)
{
	FPixel2DAnimNode_Base::Initialize_AnyThread(Context);

	const UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintClass = Context.GetAnimClass();

	if (const FBakedAnimationStateMachine* Machine = GetMachineDescription())
	{
		ElapsedTime = 0.0f;

		CurrentState = INDEX_NONE;

		if (Machine->States.Num() > 0)
		{
			// Create a pose link for each state we can reach
			StatePoseLinks.Reset();
			StatePoseLinks.Reserve(Machine->States.Num());
			for (int32 StateIndex = 0; StateIndex < Machine->States.Num(); ++StateIndex)
			{
				const FBakedAnimationState& State = Machine->States[StateIndex];
				FPixel2DPoseLink* StatePoseLink = new (StatePoseLinks) FPixel2DPoseLink();

				// because conduits don't contain bound graphs, this link is no longer guaranteed to be valid
				if (State.StateRootNodeIndex != INDEX_NONE)
				{
					StatePoseLink->LinkID = AnimBlueprintClass->GetAnimNodeProperties().Num() - 1 - State.StateRootNodeIndex; // REVISIT
				}

				// also initialize transitions
				if (State.EntryRuleNodeIndex != INDEX_NONE)
				{
					if (FPixel2DAnimNode_TransitionResult* TransitionNode = GetSpriteNodeFromPropertyIndex<FPixel2DAnimNode_TransitionResult>(Context.AnimInstanceProxy->GetAnimInstanceObject(), AnimBlueprintClass, State.EntryRuleNodeIndex))
					{
						TransitionNode->Initialize_AnyThread(Context);
					}
				}

				for (int32 TransitionIndex = 0; TransitionIndex < State.Transitions.Num(); ++TransitionIndex)
				{
					const FBakedStateExitTransition& TransitionRule = State.Transitions[TransitionIndex];
					if (TransitionRule.CanTakeDelegateIndex != INDEX_NONE)
					{
						if (FPixel2DAnimNode_TransitionResult* TransitionNode = GetSpriteNodeFromPropertyIndex<FPixel2DAnimNode_TransitionResult>(Context.AnimInstanceProxy->GetAnimInstanceObject(), AnimBlueprintClass, TransitionRule.CanTakeDelegateIndex))
						{
							TransitionNode->Initialize_AnyThread(Context);
						}
					}
				}
			}

			// Move to the default state
			SetState(Context, Machine->InitialState);

			// initialize first update
			bFirstUpdate = true;
		}
	}
}

const FBakedAnimationState& FPixel2DAnimNode_StateMachine::GetStateInfo() const
{
	return PRIVATE_MachineDescription->States[CurrentState];
}

const FBakedAnimationState& FPixel2DAnimNode_StateMachine::GetStateInfo(int32 StateIndex) const
{
	return PRIVATE_MachineDescription->States[StateIndex];
}

const int32 FPixel2DAnimNode_StateMachine::GetStateIndex(const FBakedAnimationState& StateInfo) const
{
	for (int32 Index = 0; Index < PRIVATE_MachineDescription->States.Num(); ++Index)
	{
		if (&PRIVATE_MachineDescription->States[Index] == &StateInfo)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}


const FAnimationTransitionBetweenStates& FPixel2DAnimNode_StateMachine::GetTransitionInfo(int32 TransIndex) const
{
	return PRIVATE_MachineDescription->Transitions[TransIndex];
}

void FPixel2DAnimNode_StateMachine::Update_AnyThread(const FPixel2DAnimationUpdateContext& Context)
{
	const FBakedAnimationStateMachine* Machine = GetMachineDescription();
	if (Machine != nullptr)
	{
		if (Machine->States.Num() == 0)
		{
			return;
		}
		else if (!Machine->States.IsValidIndex(CurrentState))
		{
			// Attempting to catch a crash where the state machine has been freed.
			// Reported as a symptom of a crash in UE-24732 for 4.10. This log message should not appear given changes to
			// re-instancing in 4.11 (see CL 2823202). If it does appear we need to spot integrate CL 2823202 (and supporting 
			// anim re-init changes, probably 2799786 & 2801372).
			UE_LOG(LogAnimation, Warning, TEXT("FSpriteAnimNode_StateMachine::Update - Invalid current state, please report. Attempting to use state %d of %d in state machine %d (ptr 0x%x)"), CurrentState, Machine->States.Num(), StateMachineIndexInClass, Machine);
			UE_LOG(LogAnimation, Warning, TEXT("\t\tWhen updating AnimInstance: %s"), *Context.AnimInstanceProxy->GetAnimInstanceObject()->GetName())

			return;
		}
	}
	else
	{
		return;
	}

	bool bFoundValidTransition = false;
	int32 TransitionCountThisFrame = 0;
	int32 TransitionIndex = INDEX_NONE;

	// Look for legal transitions to take; can move across multiple states in one frame (up to MaxTransitionsPerFrame)
	do
	{
		bFoundValidTransition = false;
		FPixel2DAnimationPotentialTransition PotentialTransition;

		{
			// Evaluate possible transitions out of this state
			//@TODO: Evaluate if a set is better than an array for the probably low N encountered here
			TArray<int32, TInlineAllocator<4>> VisitedStateIndices;
			FindValidTransition(Context, GetStateInfo(), /*Out*/ PotentialTransition, /*Out*/ VisitedStateIndices);
		}

		// If transition is valid and not waiting on other conditions
		if (PotentialTransition.IsValid())
		{
			bFoundValidTransition = true;

			const int32 PreviousState = CurrentState;
			const int32 NextState = PotentialTransition.TargetState;

			SetState(Context, NextState);

			TransitionCountThisFrame++;
		}
	} while (bFoundValidTransition && (TransitionCountThisFrame < MaxTransitionsPerFrame));

	StatePoseLinks[CurrentState].Update(Context);

	ElapsedTime += Context.GetDeltaTime();
}

bool FPixel2DAnimNode_StateMachine::FindValidTransition(const FPixel2DAnimationUpdateContext& Context, const FBakedAnimationState& StateInfo, /*out*/ FPixel2DAnimationPotentialTransition& OutPotentialTransition, /*out*/ TArray<int32, TInlineAllocator<4>>& OutVisitedStateIndices)
{
	// There is a possibility we'll revisit states connected through conduits,
	// so we can avoid doing unnecessary work (and infinite loops) by caching off states we have already checked
	const int32 CheckingStateIndex = GetStateIndex(StateInfo);
	if (OutVisitedStateIndices.Contains(CheckingStateIndex))
	{
		return false;
	}
	OutVisitedStateIndices.Add(CheckingStateIndex);

	const UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintClass = Context.GetAnimClass();

	// Conduit 'states' have an additional entry rule which must be true to consider taking any transitions via the conduit
	//@TODO: It would add flexibility to be able to define this on normal state nodes as well, assuming the dual-graph editing is sorted out
	if (FPixel2DAnimNode_TransitionResult* StateEntryRuleNode = GetSpriteNodeFromPropertyIndex<FPixel2DAnimNode_TransitionResult>(Context.AnimInstanceProxy->GetAnimInstanceObject(), AnimBlueprintClass, StateInfo.EntryRuleNodeIndex))
	{
		if (StateEntryRuleNode->NativeTransitionDelegate.IsBound())
		{
			// attempt to evaluate native rule
			StateEntryRuleNode->bCanEnterTransition = StateEntryRuleNode->NativeTransitionDelegate.Execute();
		}
		else
		{
			// Execute it and see if we can take this rule
			StateEntryRuleNode->EvaluateGraphExposedInputs.Execute(Context);
		}

		// not ok, back out
		if (!StateEntryRuleNode->bCanEnterTransition)
		{
			return false;
		}
	}

	const int32 NumTransitions = StateInfo.Transitions.Num();
	for (int32 TransitionIndex = 0; TransitionIndex < NumTransitions; ++TransitionIndex)
	{
		const FBakedStateExitTransition& TransitionRule = StateInfo.Transitions[TransitionIndex];
		if (TransitionRule.CanTakeDelegateIndex == INDEX_NONE)
		{
			continue;
		}

		FPixel2DAnimNode_TransitionResult* ResultNode = GetSpriteNodeFromPropertyIndex<FPixel2DAnimNode_TransitionResult>(Context.AnimInstanceProxy->GetAnimInstanceObject(), AnimBlueprintClass, TransitionRule.CanTakeDelegateIndex);

		if (ResultNode->NativeTransitionDelegate.IsBound())
		{
			// attempt to evaluate native rule
			ResultNode->bCanEnterTransition = ResultNode->NativeTransitionDelegate.Execute();
		}
		else if (TransitionRule.bAutomaticRemainingTimeRule)
		{
			bool bCanEnterTransition = false;
			ResultNode->bCanEnterTransition = true;
		}
		else
		{
			// Execute it and see if we can take this rule
			ResultNode->EvaluateGraphExposedInputs.Execute(Context);
		}

		if (ResultNode->bCanEnterTransition == TransitionRule.bDesiredTransitionReturnValue)
		{
			int32 NextState = GetTransitionInfo(TransitionRule.TransitionIndex).NextState;
			const FBakedAnimationState& NextStateInfo = GetStateInfo(NextState);

			{
				// clear out any potential transition we already have
				OutPotentialTransition.Clear();

				// fill out the potential transition information
				OutPotentialTransition.TransitionRule = &TransitionRule;
				OutPotentialTransition.TargetState = NextState;

				OutPotentialTransition.SourceTransitionIndices.Add(TransitionRule.TransitionIndex);

				return true;
			}
		}
	}

	return false;
}

void FPixel2DAnimNode_StateMachine::Evaluate_AnyThread(FPixel2DPoseContext& Output)
{
	if (const FBakedAnimationStateMachine* Machine = GetMachineDescription())
	{
		if (Machine->States.Num() == 0 || !Machine->States.IsValidIndex(CurrentState))
		{
			return;
		}
	}
	else
	{
		return;
	}

	{
		// Evaluate the current state
		StatePoseLinks[CurrentState].Evaluate(Output);
	}
}

void FPixel2DAnimNode_StateMachine::SetStateInternal(int32 NewStateIndex)
{
	checkSlow(PRIVATE_MachineDescription);
	CurrentState = FMath::Clamp<int32>(NewStateIndex, 0, PRIVATE_MachineDescription->States.Num() - 1);
	check(CurrentState == NewStateIndex);
	ElapsedTime = 0.0f;
}

void FPixel2DAnimNode_StateMachine::SetState(const FPixel2DAnimationBaseContext& Context, int32 NewStateIndex)
{
	if (NewStateIndex != CurrentState)
	{
		const int32 PrevStateIndex = CurrentState;
		bool bForceReset = false;

		if (PRIVATE_MachineDescription->States.IsValidIndex(NewStateIndex))
		{
			const FBakedAnimationState& BakedCurrentState = PRIVATE_MachineDescription->States[NewStateIndex];
			bForceReset = BakedCurrentState.bAlwaysResetOnEntry;
		}

		SetStateInternal(NewStateIndex);

		{
			// Initialize the new state since it's not part of an active transition (and thus not still initialized)
			FPixel2DAnimationInitializeContext InitContext(Context.AnimInstanceProxy);
			StatePoseLinks[NewStateIndex].Initialize(InitContext);
		}
	}
}

bool FPixel2DAnimNode_StateMachine::IsValidTransitionIndex(int32 TransitionIndex) const
{
	return PRIVATE_MachineDescription->Transitions.IsValidIndex(TransitionIndex);
}

void FPixel2DAnimNode_StateMachine::CacheMachineDescription(UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintClass)
{
	PRIVATE_MachineDescription = AnimBlueprintClass->GetBakedStateMachines().IsValidIndex(StateMachineIndexInClass) ? &(AnimBlueprintClass->GetBakedStateMachines()[StateMachineIndexInClass]) : nullptr;
}
