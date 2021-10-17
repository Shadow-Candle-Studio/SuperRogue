// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimInstanceProxy.h"
#include "Pixel2DAnimNode_Base.h"
#include "Pixel2DAnimNode_StateMachine.h"
#include "Engine/Engine.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnimInstance"

void FPixel2DAnimInstanceProxy::UpdateAnimationNode(float DeltaSeconds)
{
	if (RootNode != nullptr)
	{
		UpdateCounter.Increment();
		RootNode->Update_AnyThread(FPixel2DAnimationUpdateContext(this, DeltaSeconds, &NotifyQueue));
	}
}

void FPixel2DAnimInstanceProxy::Initialize(UPixel2DAnimInstance* InAnimInstance)
{
	// copy anim instance object if it has not already been set up
	AnimInstanceObject = InAnimInstance;

	AnimClassInterface = Cast<UPixel2DAnimBlueprintGeneratedClass>(InAnimInstance->GetClass());

	if (UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintClass = Cast<UPixel2DAnimBlueprintGeneratedClass>(InAnimInstance->GetClass()))
	{
		// Grab a pointer to the root node
		if (FStructProperty* RootAnimNodeProperty = AnimBlueprintClass->GetRootAnimNodeProperty())
		{
			RootNode = RootAnimNodeProperty->ContainerPtrToValuePtr<FPixel2DAnimNode_Base>(InAnimInstance);
		}
		else
		{
			RootNode = nullptr;
		}
	}
}

void FPixel2DAnimInstanceProxy::InitializeRootNode()
{
	if (RootNode != nullptr)
	{
		auto InitializeNode = [this](FPixel2DAnimNode_Base* AnimNode)
		{
			// Force our functions to be re-evaluated - this reinitialization may have been a 
			// consequence of our class being recompiled and functions will be invalid in that
			// case.
			AnimNode->EvaluateGraphExposedInputs.bInitialized = false;
			AnimNode->EvaluateGraphExposedInputs.Initialize(AnimNode, AnimInstanceObject);
		};

		if (AnimClassInterface)
		{
			// cache any state machine descriptions we have
			for (FStructProperty* Property : AnimClassInterface->GetAnimNodeProperties())
			{
				if (Property->Struct->IsChildOf(FPixel2DAnimNode_Base::StaticStruct()))
				{
					FPixel2DAnimNode_Base* AnimNode = Property->ContainerPtrToValuePtr<FPixel2DAnimNode_Base>(AnimInstanceObject);
					if (AnimNode)
					{
						InitializeNode(AnimNode);

						if (Property->Struct->IsChildOf(FPixel2DAnimNode_StateMachine::StaticStruct()))
						{
							FPixel2DAnimNode_StateMachine* StateMachine = static_cast<FPixel2DAnimNode_StateMachine*>(AnimNode);
							StateMachine->CacheMachineDescription(AnimClassInterface);
						}
					}
				}
			}
		}

		FPixel2DAnimationInitializeContext InitContext(this);
		RootNode->Initialize_AnyThread(InitContext);
	}
}

void FPixel2DAnimInstanceProxy::Uninitialize(UPixel2DAnimInstance* InAnimInstance)
{
}

void FPixel2DAnimInstanceProxy::PreUpdate(UPixel2DAnimInstance* InAnimInstance, float DeltaSeconds)
{
	CurrentDeltaSeconds = DeltaSeconds;

	NotifyQueue.Reset();
}

void FPixel2DAnimInstanceProxy::PostUpdate(UPixel2DAnimInstance* InAnimInstance) const
{
	InAnimInstance->NotifyQueue.Append(NotifyQueue);
}

void FPixel2DAnimInstanceProxy::TickAssetPlayerInstances(float DeltaSeconds)
{
}

FPixel2DAnimNode_Base* FPixel2DAnimInstanceProxy::GetNodeFromIndexUntyped(int32 NodeIdx, UScriptStruct* RequiredStructType)
{
	FPixel2DAnimNode_Base* NodePtr = nullptr;
	if (AnimClassInterface)
	{
		const TArray<FStructProperty*>& AnimNodeProperties = AnimClassInterface->GetAnimNodeProperties();
		const int32 InstanceIdx = AnimNodeProperties.Num() - 1 - NodeIdx;

		if (AnimNodeProperties.IsValidIndex(InstanceIdx))
		{
			FStructProperty* NodeProperty = AnimNodeProperties[InstanceIdx];

			if (NodeProperty->Struct->IsChildOf(RequiredStructType))
			{
				NodePtr = NodeProperty->ContainerPtrToValuePtr<FPixel2DAnimNode_Base>(AnimInstanceObject);
			}
		}
	}

	return NodePtr;
}

void FPixel2DAnimInstanceProxy::UpdateAnimation()
{
	// update native update
	{
		Update(CurrentDeltaSeconds);
	}

	// update all nodes
	UpdateAnimationNode(CurrentDeltaSeconds);

	// tick all our active asset players
	TickAssetPlayerInstances(CurrentDeltaSeconds);
}

void FPixel2DAnimInstanceProxy::EvaluateAnimation(FPixel2DPoseContext& Output)
{
	EvaluateAnimationNode(Output);
}

void FPixel2DAnimInstanceProxy::EvaluateAnimationNode(FPixel2DPoseContext& Output)
{
	if (RootNode != NULL)
	{
		RootNode->Evaluate_AnyThread(Output);
	}
}

float FPixel2DAnimInstanceProxy::GetInstanceCurrentStateElapsedTime(int32 MachineIndex)
{
	if (FPixel2DAnimNode_StateMachine* MachineInstance = GetStateMachineInstance(MachineIndex))
	{
		return MachineInstance->GetCurrentStateElapsedTime();
	}

	return 0.0f;
}

float FPixel2DAnimInstanceProxy::GetRelevantAnimTimeRemaining(int32 MachineIndex, int32 StateIndex)
{
	if (FPixel2DAnimNode_AssetSprite* AssetPlayer = GetRelevantAssetPlayerFromState(MachineIndex, StateIndex))
	{
		if (AssetPlayer->GetAnimAsset())
		{
			return AssetPlayer->GetCurrentAssetLength() - GetInstanceCurrentStateElapsedTime(MachineIndex);
		}
	}

	return MAX_flt;
}

float FPixel2DAnimInstanceProxy::GetRelevantAnimTimeRemainingFraction(int32 MachineIndex, int32 StateIndex)
{
	if (FPixel2DAnimNode_AssetSprite* AssetPlayer = GetRelevantAssetPlayerFromState(MachineIndex, StateIndex))
	{
		if (AssetPlayer->GetAnimAsset())
		{
			float Length = AssetPlayer->GetCurrentAssetLength();
			if (Length > 0.0f)
			{
				return (Length - GetInstanceCurrentStateElapsedTime(MachineIndex)) / Length;
			}
		}
	}

	return 1.0f;
}

float FPixel2DAnimInstanceProxy::GetRelevantAnimLength(int32 MachineIndex, int32 StateIndex)
{
	if (FPixel2DAnimNode_AssetSprite* AssetPlayer = GetRelevantAssetPlayerFromState(MachineIndex, StateIndex))
	{
		if (AssetPlayer->GetAnimAsset())
		{
			return AssetPlayer->GetCurrentAssetLength();
		}
	}

	return 0.0f;
}

float FPixel2DAnimInstanceProxy::GetRelevantAnimTime(int32 MachineIndex, int32 StateIndex)
{
	return GetInstanceCurrentStateElapsedTime(MachineIndex);
}

float FPixel2DAnimInstanceProxy::GetRelevantAnimTimeFraction(int32 MachineIndex, int32 StateIndex)
{
	if (FPixel2DAnimNode_AssetSprite* AssetPlayer = GetRelevantAssetPlayerFromState(MachineIndex, StateIndex))
	{
		float Length = AssetPlayer->GetCurrentAssetLength();
		if (Length > 0.0f)
		{
			return GetInstanceCurrentStateElapsedTime(MachineIndex) / Length;
		}
	}

	return 0.0f;
}

FPixel2DAnimNode_AssetSprite* FPixel2DAnimInstanceProxy::GetRelevantAssetPlayerFromState(int32 MachineIndex, int32 StateIndex)
{
	FPixel2DAnimNode_AssetSprite* ResultPlayer = nullptr;
	if (FPixel2DAnimNode_StateMachine* MachineInstance = GetStateMachineInstance(MachineIndex))
	{
		float MaxWeight = 0.0f;
		const FBakedAnimationState& State = MachineInstance->GetStateInfo(StateIndex);
		for (const int32& PlayerIdx : State.PlayerNodeIndices)
		{
			if (FPixel2DAnimNode_AssetSprite* Player = GetNodeFromIndex<FPixel2DAnimNode_AssetSprite>(PlayerIdx))
			{
				ResultPlayer = Player;
			}
		}
	}
	return ResultPlayer;
}

FPixel2DAnimNode_StateMachine* FPixel2DAnimInstanceProxy::GetStateMachineInstance(int32 MachineIndex)
{
	if (AnimClassInterface)
	{
		const TArray<FStructProperty*>& AnimNodeProperties = AnimClassInterface->GetAnimNodeProperties();
		if ((MachineIndex >= 0) && (MachineIndex < AnimNodeProperties.Num()))
		{
			const int32 InstancePropertyIndex = AnimNodeProperties.Num() - 1 - MachineIndex;

			FStructProperty* MachineInstanceProperty = AnimNodeProperties[InstancePropertyIndex];
			checkSlow(MachineInstanceProperty->Struct->IsChildOf(FPixel2DAnimNode_StateMachine::StaticStruct()));

			return MachineInstanceProperty->ContainerPtrToValuePtr<FPixel2DAnimNode_StateMachine>(AnimInstanceObject);
		}
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
