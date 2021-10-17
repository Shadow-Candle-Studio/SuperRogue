// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Pixel2DAnimBlueprint.h"
#include "Pixel2DAnimNotifyQueue.h"
#include "Pixel2DAnimInstance.h"
#include "Pixel2DAnimBlueprintGeneratedClass.h"
#include "Pixel2DAnimNode_AssetSprite.h"

#include "Pixel2DAnimInstanceProxy.generated.h"

struct FPixel2DAnimNode_AssetSprite;
struct FPixel2DAnimNode_Base;
struct FPixel2DAnimNode_StateMachine;
struct FPixel2DPoseContext;

/** Proxy object passed around during animation tree update in lieu of a UAnimInstance */
USTRUCT(meta = (DisplayName = "Native Variables"))
struct PIXEL2D_API FPixel2DAnimInstanceProxy
{
	GENERATED_BODY()

public:
	FPixel2DAnimInstanceProxy()
		: AnimInstanceObject(nullptr)
		, CurrentDeltaSeconds(0.0f)
		, RootNode(nullptr)
	{
	}

	FPixel2DAnimInstanceProxy(UPixel2DAnimInstance* Instance)
		: AnimInstanceObject(Instance)
		, CurrentDeltaSeconds(0.0f)
		, RootNode(nullptr)
	{
	}

	virtual ~FPixel2DAnimInstanceProxy() {}

	UPixel2DAnimBlueprintGeneratedClass* GetAnimClassInterface() const
	{
		return AnimClassInterface;
	}

	/** Get the last DeltaSeconds passed into PreUpdate() */
	float GetDeltaSeconds() const
	{
		return CurrentDeltaSeconds;
	}

	/** Tick active asset players */
	void TickAssetPlayerInstances(float DeltaSeconds);

	/** Access UObject base of UPixel2DAnimInstance */
	UObject* GetAnimInstanceObject() { return AnimInstanceObject; }
	const UObject* GetAnimInstanceObject() const { return AnimInstanceObject; }

	/** Gets an unchecked (can return nullptr) node given an index into the node property array */
	FPixel2DAnimNode_Base* GetNodeFromIndexUntyped(int32 NodeIdx, UScriptStruct* RequiredStructType);

	/** Gets an unchecked (can return nullptr) node given an index into the node property array */
	template<class NodeType>
	NodeType* GetNodeFromIndex(int32 NodeIdx)
	{
		return (NodeType*)GetNodeFromIndexUntyped(NodeIdx, NodeType::StaticStruct());
	}

	/** Check whether we have a valid root node */
	bool HasRootNode() const
	{
		return RootNode != nullptr;
	}

	/** Only restricted classes can access the protected interface */
	friend class UPixel2DAnimInstance;

protected:
	/** Called when our anim instance is being initialized */
	virtual void Initialize(UPixel2DAnimInstance* InAnimInstance);

	/** Called when our anim instance is being uninitialized */
	virtual void Uninitialize(UPixel2DAnimInstance* InAnimInstance);

	/** Called before update so we can copy any data we need */
	virtual void PreUpdate(UPixel2DAnimInstance* InAnimInstance, float DeltaSeconds);

	/** Update override point */
	virtual void Update(float DeltaSeconds) {}

	/** Updates the anim graph */
	virtual void UpdateAnimationNode(float DeltaSeconds);

	/** Called after update so we can copy any data we need */
	virtual void PostUpdate(UPixel2DAnimInstance* InAnimInstance) const;

	/** Calls Update(), updates the anim graph, ticks asset players */
	void UpdateAnimation();

	/** Evaluates the anim graph if Evaluate() returns false */
	void EvaluateAnimation(FPixel2DPoseContext& Output);

	/** Evaluates the anim graph */
	void EvaluateAnimationNode(FPixel2DPoseContext& Output);

	/** Gets the most relevant asset player in a specified state */
	FPixel2DAnimNode_AssetSprite* GetRelevantAssetPlayerFromState(int32 MachineIndex, int32 StateIndex);

	/** Gets the runtime instance of the specified state machine */
	FPixel2DAnimNode_StateMachine* GetStateMachineInstance(int32 MachineIndex);

	/** Get the current elapsed time of a state within the specified state machine */
	float GetInstanceCurrentStateElapsedTime(int32 MachineIndex);

	/** Get the time remaining in seconds for the most relevant animation in the source state */
	float GetRelevantAnimTimeRemaining(int32 MachineIndex, int32 StateIndex);

	/** Get the time remaining as a fraction of the duration for the most relevant animation in the source state */
	float GetRelevantAnimTimeRemainingFraction(int32 MachineIndex, int32 StateIndex);

	/** Get the length in seconds of the most relevant animation in the source state */
	float GetRelevantAnimLength(int32 MachineIndex, int32 StateIndex);

	/** Get the current accumulated time in seconds for the most relevant animation in the source state */
	float GetRelevantAnimTime(int32 MachineIndex, int32 StateIndex);

	/** Get the current accumulated time as a fraction of the length of the most relevant animation in the source state */
	float GetRelevantAnimTimeFraction(int32 MachineIndex, int32 StateIndex);

	/** Initialize the root node - split into a separate function for backwards compatibility (initialization order) reasons */
	void InitializeRootNode();

	/** Object ptr to our UPixel2DAnimInstance */
	mutable UObject* AnimInstanceObject;

	/** Our anim blueprint generated class */
	UPixel2DAnimBlueprintGeneratedClass* AnimClassInterface;

	/** The last time passed into PreUpdate() */
	float CurrentDeltaSeconds;

	/** Anim graph */
	FPixel2DAnimNode_Base* RootNode;

	/** Animation Notifies that has been triggered in the latest tick **/
	FPixel2DAnimNotifyQueue NotifyQueue;

protected:
	// Counters for synchronization
	FGraphTraversalCounter InitializationCounter;
	FGraphTraversalCounter UpdateCounter;
	FGraphTraversalCounter EvaluationCounter;
};
