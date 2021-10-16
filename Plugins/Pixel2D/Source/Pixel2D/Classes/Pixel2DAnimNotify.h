// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "Pixel2DAnimNotifyEvent.h"
#include "Pixel2DAnimNotify.generated.h"

USTRUCT()
struct FPixel2DAnimNotifyEventArrayElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = NotifyEvent)
	bool bEnabled = false;

	UPROPERTY(EditAnywhere, Category = NotifyEvent)
	FName NotifyName = FName(TEXT("Pixel2D_AnimNotify"));

	float NormalisedTriggerOffset = 0;
};

// Proxy class for Pixel2D notifies
UCLASS(Blueprintable, const, hidecategories = Object, collapsecategories)
class PIXEL2D_API UPixel2DAnimNotify : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = NotifyEvent)
	TArray<FPixel2DAnimNotifyEventArrayElement> NotificationEvents;

	TArray<FPixel2DAnimNotifyEvent> * NotifyArray;

	void Init(TArray<FPixel2DAnimNotifyEvent> & InNotifyArray)
	{
		NotifyArray = &InNotifyArray;
		NotificationEvents.Empty();

		for (auto NotifyEvent : *NotifyArray)
		{
			FPixel2DAnimNotifyEventArrayElement elem;
			elem.bEnabled = NotifyEvent.bEnabled;
			elem.NotifyName = NotifyEvent.NotifyName;
			elem.NormalisedTriggerOffset = NotifyEvent.NormalisedTriggerOffset;
			NotificationEvents.Push(elem);
		}
	}

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
	{
		NotifyArray->Empty();

		int TotalEvents = NotificationEvents.Num();
		for (int i = 0; i < NotificationEvents.Num(); i++)
		{
			FPixel2DAnimNotifyEventArrayElement Notify = NotificationEvents[i];
			if (Notify.NotifyName.IsNone())
			{
				Notify.NotifyName = FName(TEXT("Pixel2D_AnimNotify"));
			}
			FPixel2DAnimNotifyEvent NewNotify;
			NewNotify.bEnabled   = Notify.bEnabled;
			NewNotify.NotifyName = Notify.NotifyName;
			NewNotify.NormalisedTriggerOffset = Notify.NormalisedTriggerOffset;
			NotifyArray->Push(NewNotify);
		}
	}
#endif
};