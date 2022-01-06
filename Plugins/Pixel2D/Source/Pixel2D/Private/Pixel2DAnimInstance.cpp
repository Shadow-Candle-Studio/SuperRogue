// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimInstance.h"
#include "Pixel2DAnimInstanceProxy.h"

#include "Pixel2DAnimNode_Base.h"

UPixel2DAnimInstance::UPixel2DAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bUpdatingAnimation(false)
	, bPostUpdatingAnimation(false)
{
	bNeedsUpdate = false;

	// Default to using threaded animation update.
	bUseMultiThreadedAnimationUpdate = true;
}

APawn* UPixel2DAnimInstance::TryGetPawnOwner() const
{
	UPixel2DComponent* OwnerComponent = GetPaperSpriteComponent();
	if (AActor* OwnerActor = OwnerComponent->GetOwner())
	{
		return Cast<APawn>(OwnerActor);
	}

	return NULL;
}

#define LOCTEXT_NAMESPACE "SpriteAnimInstance"

AActor* UPixel2DAnimInstance::GetOwningActor() const
{
	return NULL;
}

UPixel2DComponent* UPixel2DAnimInstance::GetOwningComponent() const
{
	return GetSpriteComponent();
}

void UPixel2DAnimInstance::InitializeAnimation()
{
	UninitializeAnimation();

	UPixel2DComponent* OwnerComponent = GetSpriteComponent();
	if (OwnerComponent->GetFlipbook() != NULL)
	{
		CurrentFlipbook = OwnerComponent->GetFlipbook();
	}
	else
	{
		CurrentFlipbook = NULL;
	}

	GetProxyOnGameThread<FPixel2DAnimInstanceProxy>().Initialize(this);
	GetProxyOnGameThread<FPixel2DAnimInstanceProxy>().InitializeRootNode();
}

void UPixel2DAnimInstance::UninitializeAnimation()
{
    GetProxyOnGameThread<FPixel2DAnimInstanceProxy>().Uninitialize(this);
	NotifyQueue.Reset();
}

void UPixel2DAnimInstance::UpdateAnimation(float DeltaSeconds, bool bNeedsValidRootMotion)
{
#if DO_CHECK
	checkf(!bUpdatingAnimation, TEXT("UpdateAnimation already in progress, circular detected for Flipbook [%s], AnimInstance [%s]"), *GetNameSafe(GetOwningComponent()), *GetName());
	TGuardValue<bool> CircularGuard(bUpdatingAnimation, true);
#endif

	// acquire the proxy as we need to update
	FPixel2DAnimInstanceProxy& Proxy = GetProxyOnGameThread<FPixel2DAnimInstanceProxy>();
	NotifyQueue.Reset();

	PreUpdateAnimation(DeltaSeconds);
	BlueprintUpdateAnimation(DeltaSeconds);

	if (bNeedsValidRootMotion || NeedsImmediateUpdate(DeltaSeconds))
	{
		// cant use parallel update, so just do the work here
		Proxy.UpdateAnimation();
		PostUpdateAnimation();
	}
}

void UPixel2DAnimInstance::PreUpdateAnimation(float DeltaSeconds)
{
	bNeedsUpdate = true;
	NotifyQueue.Reset();

	GetProxyOnGameThread<FPixel2DAnimInstanceProxy>().PreUpdate(this, DeltaSeconds);
}

void UPixel2DAnimInstance::PostUpdateAnimation()
{
	bNeedsUpdate = false;

	// acquire the proxy as we need to update
	FPixel2DAnimInstanceProxy& Proxy = GetProxyOnGameThread<FPixel2DAnimInstanceProxy>();

	Proxy.PostUpdate(this);
}

void UPixel2DAnimInstance::DispatchQueuedAnimEvents()
{
	// now trigger Notifies
	TriggerAnimNotifies(GetProxyOnGameThread<FPixel2DAnimInstanceProxy>().GetDeltaSeconds());
}

bool UPixel2DAnimInstance::NeedsImmediateUpdate(float DeltaSeconds) const
{
	return true;
}

bool UPixel2DAnimInstance::NeedsUpdate() const
{
	return bNeedsUpdate;
}

bool UPixel2DAnimInstance::ParallelCanEvaluate(const UPixel2DComponent* InSkeletalMesh) const
{
	return true;
}

UPaperFlipbook* UPixel2DAnimInstance::ParallelEvaluateAnimation(const UPixel2DComponent* InSkeletalMesh, UPaperFlipbook * Flipbook)
{
	FPixel2DAnimInstanceProxy& Proxy = GetProxyOnAnyThread<FPixel2DAnimInstanceProxy>();

	FMemMark Mark(FMemStack::Get());

	FPixel2DPoseContext EvaluationContext(&Proxy);

	// Run the anim blueprint
	Proxy.EvaluateAnimation(EvaluationContext);
	Flipbook = EvaluationContext.Flipbook;

	return Flipbook;
}

bool UPixel2DAnimInstance::CanTransitionSignature() const
{
	return false;
}

void UPixel2DAnimInstance::PostInitProperties()
{
	Super::PostInitProperties();

	if (AnimInstanceProxy == nullptr)
	{
		AnimInstanceProxy = CreateAnimInstanceProxy();
		check(AnimInstanceProxy != nullptr);
	}
}

void UPixel2DAnimInstance::TriggerAnimNotifies(float DeltaSeconds)
{
	for (int32 Index = 0; Index<NotifyQueue.AnimNotifies.Num(); Index++)
	{
		const FAnimNotifyEvent* AnimNotifyEvent = NotifyQueue.AnimNotifies[Index].GetNotify();

		if (!AnimNotifyEvent)
			continue;

		// AnimNotifyState
		if (AnimNotifyEvent->NotifyStateClass)
		{
			continue;
		}

		// Trigger non 'state' AnimNotifies
		TriggerSingleAnimNotify(AnimNotifyEvent);
	}
}

void UPixel2DAnimInstance::TriggerSingleAnimNotify(const FAnimNotifyEvent* AnimNotifyEvent)
{
	// This is for non 'state' anim notifies.
	if (AnimNotifyEvent && (AnimNotifyEvent->NotifyStateClass == NULL))
	{
		if (AnimNotifyEvent->Notify != NULL)
		{
		}
		else if (AnimNotifyEvent->NotifyName != NAME_None)
		{
			
			// Custom Event based notifies. These will call a AnimNotify_* function on the AnimInstance.
			UFunction* Function = FindFunction(AnimNotifyEvent->NotifyName);
			if (Function)
			{
				// if parameter is none, add event
				if (Function->NumParms == 0)
				{
					ProcessEvent(Function, NULL);
				}
				else
				{
					// Actor has event, but with different parameters. Print warning
					//UE_LOG(LogSpriteAnimEditor, Warning, TEXT("Pixel 2DAnim notifier named %s, but the parameter number does not match or not of the correct type"), *FuncName);
				}
			}
		}
	}
}
 
float UPixel2DAnimInstance::GetInstanceCurrentStateElapsedTime(int32 MachineIndex)
{
	return GetProxyOnAnyThread<FPixel2DAnimInstanceProxy>().GetInstanceCurrentStateElapsedTime(MachineIndex);
}

float UPixel2DAnimInstance::GetRelevantAnimTimeRemaining(int32 MachineIndex, int32 StateIndex)
{
	return GetProxyOnAnyThread<FPixel2DAnimInstanceProxy>().GetRelevantAnimTimeRemaining(MachineIndex, StateIndex);
}

float UPixel2DAnimInstance::GetRelevantAnimTimeRemainingFraction(int32 MachineIndex, int32 StateIndex)
{
	return GetProxyOnAnyThread<FPixel2DAnimInstanceProxy>().GetRelevantAnimTimeRemainingFraction(MachineIndex, StateIndex);
}

float UPixel2DAnimInstance::GetRelevantAnimLength(int32 MachineIndex, int32 StateIndex)
{
	return GetProxyOnAnyThread<FPixel2DAnimInstanceProxy>().GetRelevantAnimLength(MachineIndex, StateIndex);
}

float UPixel2DAnimInstance::GetRelevantAnimTime(int32 MachineIndex, int32 StateIndex)
{
	return GetProxyOnAnyThread<FPixel2DAnimInstanceProxy>().GetRelevantAnimTime(MachineIndex, StateIndex);
}

float UPixel2DAnimInstance::GetRelevantAnimTimeFraction(int32 MachineIndex, int32 StateIndex)
{
	return GetProxyOnAnyThread<FPixel2DAnimInstanceProxy>().GetRelevantAnimTimeFraction(MachineIndex, StateIndex);
}

bool UPixel2DAnimInstance::IsRunningParallelEvaluation() const
{
	return false;
}
//
FPixel2DAnimInstanceProxy* UPixel2DAnimInstance::CreateAnimInstanceProxy()
{
	return new FPixel2DAnimInstanceProxy(this);
}

void UPixel2DAnimInstance::DestroyAnimInstanceProxy(FPixel2DAnimInstanceProxy* InProxy)
{
	delete InProxy;
}

void UPixel2DAnimInstance::PreInitializeRootNode()
{
	// This function should only be called on the CDO
	check(HasAnyFlags(RF_ClassDefaultObject));

	UPixel2DAnimBlueprintGeneratedClass* AnimClassInterface = Cast<UPixel2DAnimBlueprintGeneratedClass>(GetClass());

	if (AnimClassInterface)
	{
		for (FStructProperty* Property : AnimClassInterface->GetAnimNodeProperties())
		{
			if (Property->Struct->IsChildOf(FPixel2DAnimNode_Base::StaticStruct()))
			{
				FPixel2DAnimNode_Base* AnimNode = Property->ContainerPtrToValuePtr<FPixel2DAnimNode_Base>(this);
				if (AnimNode)
				{
					AnimNode->EvaluateGraphExposedInputs.Initialize(AnimNode, this);
				}
			}
		}
	}
}
#undef LOCTEXT_NAMESPACE 
