// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "Pixel2DComponent.h"
#include "Animation/AnimInstance.h"
#include "Pixel2DAnimNotifyQueue.h"
#include "Pixel2DAnimInstance.generated.h"

struct FPixel2DAnimInstanceProxy;

UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType, meta = (BlueprintThreadSafe))
class PIXEL2D_API UPixel2DAnimInstance : public UObject
{
	GENERATED_UCLASS_BODY()

	// Disable compiler-generated deprecation warnings by implementing our own destructor
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	~UPixel2DAnimInstance() {}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

	/** Flag to check back on the game thread that indicates we need to run PostUpdateAnimation() in the post-eval call */
	bool bNeedsUpdate;


	public:

		// @todo document
		inline UPixel2DComponent* GetPaperSpriteComponent() const { return CastChecked<UPixel2DComponent>(GetOuter()); }

	public:
		UFUNCTION(BlueprintCallable, Category = "Animation", meta = (NotBlueprintThreadSafe))
		virtual APawn* TryGetPawnOwner() const;

		/** This is used to extract animation. If Mesh exists, this will be overwritten by Mesh->Skeleton */
		UPROPERTY(transient)
		UPaperFlipbook * CurrentFlipbook;

		UPROPERTY(meta = (BlueprintCompilerGeneratedDefaults))
		bool bUseMultiThreadedAnimationUpdate;

		// Are we being evaluated on a worker thread
		bool IsRunningParallelEvaluation() const;

		// Can does this anim instance need an update (parallel or not)?
		bool NeedsUpdate() const;

	private:
		// Does this anim instance need immediate update (rather than parallel)?
		bool NeedsImmediateUpdate(float DeltaSeconds) const;

	public:
		/** Returns the owning actor of this AnimInstance */
		UFUNCTION(BlueprintCallable, Category = "Animation", meta = (NotBlueprintThreadSafe))
		AActor* GetOwningActor() const;

		// Returns the skeletal mesh component that has created this AnimInstance
		UFUNCTION(BlueprintCallable, Category = "Animation", meta = (NotBlueprintThreadSafe))
		UPixel2DComponent* GetOwningComponent() const;

	public:

		/** Executed when the Animation is initialized */
		UFUNCTION(BlueprintImplementableEvent)
		void BlueprintInitializeAnimation();

		/** Executed when the Animation is updated */
		UFUNCTION(BlueprintImplementableEvent)
		void BlueprintUpdateAnimation(float DeltaTimeX);

		/** Executed after the Animation is evaluated */
		UFUNCTION(BlueprintImplementableEvent)
		void BlueprintPostEvaluateAnimation();

		/** Executed when begin play is called on the owning component */
		UFUNCTION(BlueprintImplementableEvent)
		void BlueprintBeginPlay();

		bool CanTransitionSignature() const;

	private:
		/** Used to guard against recursive calls to UpdateAnimation */
		bool bUpdatingAnimation;

		/** Used to guard against recursive calls to UpdateAnimation */
		bool bPostUpdatingAnimation;

	public:

		/** Is this animation currently running post update */
		bool IsPostUpdatingAnimation() const { return bPostUpdatingAnimation; }

		/** Get the current elapsed time of a state within the specified state machine */
		UFUNCTION(BlueprintPure, Category = "AnimTime", meta = (DisplayName = "Current State Time", BlueprintInternalUseOnly = "true", AnimGetter = "true", GetterContext = "Transition"))
		float GetInstanceCurrentStateElapsedTime(int32 MachineIndex);

		/** Get the time remaining in seconds for the most relevant animation in the source state */
		UFUNCTION(BlueprintPure, Category = "AnimTime", meta = (BlueprintInternalUseOnly = "true", AnimGetter = "true", GetterContext = "Transition"))
		float GetRelevantAnimTimeRemaining(int32 MachineIndex, int32 StateIndex);

		/** Get the time remaining as a fraction of the duration for the most relevant animation in the source state */
		UFUNCTION(BlueprintPure, Category = "AnimTime", meta = (BlueprintInternalUseOnly = "true", AnimGetter = "true", GetterContext = "Transition"))
		float GetRelevantAnimTimeRemainingFraction(int32 MachineIndex, int32 StateIndex);

		/** Get the length in seconds of the most relevant animation in the source state */
		UFUNCTION(BlueprintPure, Category = "AnimTime", meta = (BlueprintInternalUseOnly = "true", AnimGetter = "true", GetterContext = "Transition"))
		float GetRelevantAnimLength(int32 MachineIndex, int32 StateIndex);

		/** Get the current accumulated time in seconds for the most relevant animation in the source state */
		UFUNCTION(BlueprintPure, Category = "AnimTime", meta = (BlueprintInternalUseOnly = "true", AnimGetter = "true", GetterContext = "Transition"))
		float GetRelevantAnimTime(int32 MachineIndex, int32 StateIndex);

		/** Get the current accumulated time as a fraction of the length of the most relevant animation in the source state */
		UFUNCTION(BlueprintPure, Category = "AnimTime", meta = (BlueprintInternalUseOnly = "true", AnimGetter = "true", GetterContext = "Transition"))
		float GetRelevantAnimTimeFraction(int32 MachineIndex, int32 StateIndex);

	public:
		//~ Begin UObject Interface
		virtual void PostInitProperties() override;
		//~ End UObject Interface

		// Animation phase trigger
		// start with initialize
		// update happens in every tick. Can happen in parallel with others if conditions are right.
		// evaluate happens when condition is met - i.e. depending on your skeletalmeshcomponent update flag
		// post eval happens after evaluation is done
		// uninitialize happens when owner is unregistered
		void InitializeAnimation();
		void UpdateAnimation(float DeltaSeconds, bool bNeedsValidRootMotion);

		/** Called after updates are completed, dispatches notifies etc. */
		void PostUpdateAnimation();

        /** Check whether evaluation can be performed on the supplied skeletal mesh. Can be called from worker threads. */
		bool ParallelCanEvaluate(const UPixel2DComponent* InSkeletalMesh) const;

		/** Perform evaluation. Can be called from worker threads. */
		UPaperFlipbook* ParallelEvaluateAnimation(const UPixel2DComponent* InSkeletalMesh, UPaperFlipbook * Flipbook);

		void UninitializeAnimation();

		/** Called on the CDO to pre-init cached UFunctions */
		void PreInitializeRootNode();

		/** Animation Notifies that has been triggered in the latest tick **/
		FPixel2DAnimNotifyQueue NotifyQueue;

	public:
		inline UPixel2DComponent* GetSpriteComponent() const { return CastChecked<UPixel2DComponent>(GetOuter()); }

		/** Trigger AnimNotifies **/
		void TriggerAnimNotifies(float DeltaSeconds);
		void TriggerSingleAnimNotify(const FAnimNotifyEvent* AnimNotifyEvent);

	protected:

		/** Called to setup for updates */
		virtual void PreUpdateAnimation(float DeltaSeconds);

		/** Override point for derived classes to create their own proxy objects (allows custom allocation) */
		virtual FPixel2DAnimInstanceProxy* CreateAnimInstanceProxy();

		/** Override point for derived classes to destroy their own proxy objects (allows custom allocation) */
		virtual void DestroyAnimInstanceProxy(FPixel2DAnimInstanceProxy* InProxy);

		/** Access the proxy but block if a task is currently in progress as it wouldn't be safe to access it */
		template <typename T /*= FPixel2DAnimInstanceProxy*/>	// @TODO: Cant default parameters to this function on Xbox One until we move off the VS2012 compiler
		FORCEINLINE T& GetProxyOnGameThread()
		{
			check(IsInGameThread());
			if (GetOuter() && GetOuter()->IsA<UPixel2DComponent>())
			{
				bool bBlockOnTask = true;
				bool bPerformPostAnimEvaluation = true;
				GetSpriteComponent()->HandleExistingParallelEvaluationTask(bBlockOnTask, bPerformPostAnimEvaluation);
			}
			if (AnimInstanceProxy == nullptr)
			{
				AnimInstanceProxy = CreateAnimInstanceProxy();
			}
			return *static_cast<T*>(AnimInstanceProxy);
		}

		/** Access the proxy but block if a task is currently in progress as it wouldn't be safe to access it */
		template <typename T/* = FPixel2DAnimInstanceProxy*/>	// @TODO: Cant default parameters to this function on Xbox One until we move off the VS2012 compiler
		FORCEINLINE const T& GetProxyOnGameThread() const
		{
			check(IsInGameThread());
			if (GetOuter() && GetOuter()->IsA<UPixel2DComponent>())
			{
				bool bBlockOnTask = true;
				bool bPerformPostAnimEvaluation = true;
				GetSpriteComponent()->HandleExistingParallelEvaluationTask(bBlockOnTask, bPerformPostAnimEvaluation);
			}
			if (AnimInstanceProxy == nullptr)
			{
				AnimInstanceProxy = const_cast<UPixel2DAnimInstance*>(this)->CreateAnimInstanceProxy();
			}
			return *static_cast<const T*>(AnimInstanceProxy);
		}

		/** Access the proxy but block if a task is currently in progress (and we are on the game thread) as it wouldn't be safe to access it */
		template <typename T/* = FPixel2DAnimInstanceProxy*/>	// @TODO: Cant default parameters to this function on Xbox One until we move off the VS2012 compiler
		FORCEINLINE T& GetProxyOnAnyThread()
		{
			if (GetOuter() && GetOuter()->IsA<UPixel2DComponent>())
			{
				if (IsInGameThread())
				{
					bool bBlockOnTask = true;
					bool bPerformPostAnimEvaluation = true;
					GetSpriteComponent()->HandleExistingParallelEvaluationTask(bBlockOnTask, bPerformPostAnimEvaluation);
				}
			}
			if (AnimInstanceProxy == nullptr)
			{
				AnimInstanceProxy = CreateAnimInstanceProxy();
			}
			return *static_cast<T*>(AnimInstanceProxy);
		}

		/** Access the proxy but block if a task is currently in progress (and we are on the game thread) as it wouldn't be safe to access it */
		template <typename T/* = FPixel2DAnimInstanceProxy*/>	// @TODO: Cant default parameters to this function on Xbox One until we move off the VS2012 compiler
		FORCEINLINE const T& GetProxyOnAnyThread() const
		{
			if (GetOuter() && GetOuter()->IsA<UPixel2DComponent>())
			{
				if (IsInGameThread())
				{
					bool bBlockOnTask = true;
					bool bPerformPostAnimEvaluation = true;
					GetSpriteComponent()->HandleExistingParallelEvaluationTask(bBlockOnTask, bPerformPostAnimEvaluation);
				}
			}
			if (AnimInstanceProxy == nullptr)
			{
				AnimInstanceProxy = const_cast<UPixel2DAnimInstance*>(this)->CreateAnimInstanceProxy();
			}
			return *static_cast<const T*>(AnimInstanceProxy);
		}

	protected:
		/** Proxy object, nothing should access this from an externally-callable API as it is used as a scratch area on worker threads */
		mutable FPixel2DAnimInstanceProxy* AnimInstanceProxy;

	public:
		/** Dispatch AnimEvents (AnimNotifies, Montage Events) queued during UpdateAnimation() */
		void DispatchQueuedAnimEvents();
};
