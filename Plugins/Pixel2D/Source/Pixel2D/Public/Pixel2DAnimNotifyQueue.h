// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Pixel2DAnimNotifyQueue.generated.h"

struct FPixel2DAnimInstanceProxy;
struct FPixel2DAnimNotifyEvent;

USTRUCT()
struct FPixel2DAnimNotifyEventReference
{
	GENERATED_BODY()

	FPixel2DAnimNotifyEventReference()
		: Notify(nullptr)
		, NotifySource(nullptr)
	{}

	FPixel2DAnimNotifyEventReference(const FPixel2DAnimNotifyEventReference& rhs)
		: Notify(rhs.Notify)
		, NotifySource(rhs.NotifySource)
	{
	}

	FPixel2DAnimNotifyEventReference(const FPixel2DAnimNotifyEvent* InNotify, const UObject* InNotifySource)
		: Notify(InNotify)
		, NotifySource(InNotifySource)
	{}

	const FPixel2DAnimNotifyEvent* GetNotify() const
	{
		return NotifySource ? Notify : nullptr;
	}

	friend bool operator==(const FPixel2DAnimNotifyEventReference& Lhs, const FPixel2DAnimNotifyEventReference& Rhs)
	{
		return Lhs.Notify == Rhs.Notify;
	}

	friend bool operator==(const FPixel2DAnimNotifyEventReference& Lhs, const FPixel2DAnimNotifyEvent& Rhs);

private:

	const FPixel2DAnimNotifyEvent* Notify;

	UPROPERTY(transient)
	const UObject* NotifySource;
};

USTRUCT()
struct FPixel2DAnimNotifyQueue
{
	GENERATED_BODY()

	/** Add notify to queue*/
	void AddAnimNotify(const FPixel2DAnimNotifyEvent* Notify, const UObject* NotifySource);

	/** Reset queue & update LOD level */
	void Reset();

	/** Append one queue to another */
	void Append(const FPixel2DAnimNotifyQueue& Queue);

	/** Animation Notifies that has been triggered in the latest tick **/
	UPROPERTY(transient)
	TArray<FPixel2DAnimNotifyEventReference> AnimNotifies;

	/** Adds the contents of the NewNotifies array to the DestArray (maintaining uniqueness of notify states*/
	void AddAnimNotifiesToDestNoFiltering(const TArray<FPixel2DAnimNotifyEventReference>& NewNotifies, TArray<FPixel2DAnimNotifyEventReference>& DestArray) const;
};
