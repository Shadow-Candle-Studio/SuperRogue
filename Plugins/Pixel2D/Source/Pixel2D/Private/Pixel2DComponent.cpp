// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DComponent.h"
#include "Pixel2DAnimInstance.h"

UPixel2DComponent::UPixel2DComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GlobalAnimRateScale = 1.0f;
}

void UPixel2DComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	InitAnim();
}

void UPixel2DComponent::InitializeComponent()
{
	Super::InitializeComponent();

	InitAnim();
}

void UPixel2DComponent::OnRegister()
{
	Super::OnRegister();

	// We force an initialization here because we're in one of two cases.
	// 1) First register, no spawned instance, need to initialize
	// 2) We're being re-registered, in which case we've went through
	// OnUnregister and unconditionally uninitialized our anim instances
	// so we need to force initialize them before we begin to tick.
	InitAnim();

	if (!FApp::CanEverRender())
	{
		SetComponentTickEnabled(false);
	}
}

void UPixel2DComponent::SetAnimInstance(UClass * NewAnimInstance)
{
	if (NewAnimInstance != SpriteAnimInstance)
	{
		AActor* ComponentOwner = GetOwner();
		
		if ((ComponentOwner == nullptr) || AreDynamicDataChangesAllowed())
		{
			SpriteAnimInstance = NewAnimInstance;
			InitAnim(/*true*/);
		}
	}
}

bool UPixel2DComponent::InitializeAnimScriptInstance()
{
	AnimScriptInstance = NewObject<UPixel2DAnimInstance>(this, SpriteAnimInstance);
	if (AnimScriptInstance)
	{
		AnimScriptInstance->InitializeAnimation();
	}
	return true;
}

bool UPixel2DComponent::NeedToSpawnAnimScriptInstance() const
{
	if (SpriteAnimInstance)
	{
		return true;
	}
	return false;
}

void UPixel2DComponent::InitAnim() 
{
	if (NeedToSpawnAnimScriptInstance())
	{
		const bool bInitializedAnimInstance = InitializeAnimScriptInstance();
		TickAnimation(0, false);
	}
}

bool UPixel2DComponent::ShouldTickAnimation() const
{
	return true;
}

void UPixel2DComponent::TickAnimation(float DeltaTime, bool bNeedsValidRootMotion)
{
	if (GetFlipbook() || !bNeedsValidRootMotion)
	{
		if (AnimScriptInstance != nullptr)
		{
			// Tick the animation
			AnimScriptInstance->UpdateAnimation(DeltaTime * GlobalAnimRateScale, bNeedsValidRootMotion);
			UPaperFlipbook * CurrentFlipbook = NULL;
			UPaperFlipbook * CurrentFlipbook1 = EvaluateAnimation(this, AnimScriptInstance, CurrentFlipbook);

			if (CurrentFlipbook1 != SourceFlipbook)
			{
				SetFlipbook(CurrentFlipbook1);
			}

			// Dispatch queued events
			AnimScriptInstance->DispatchQueuedAnimEvents();
		}
	}
}

void UPixel2DComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	TickAnimation(DeltaTime, false);
}

bool UPixel2DComponent::HandleExistingParallelEvaluationTask(bool bBlockOnTask, bool bPerformPostAnimEvaluation)
{
	return false;
}

UPaperFlipbook * UPixel2DComponent::EvaluateAnimation(const UPixel2DComponent* InSkeletalMesh, UPixel2DAnimInstance* InAnimInstance, UPaperFlipbook * OutPose) const
{
	return InAnimInstance->ParallelEvaluateAnimation(InSkeletalMesh, OutPose);
}