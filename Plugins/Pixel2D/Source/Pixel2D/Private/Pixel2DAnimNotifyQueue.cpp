// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNotifyQueue.h"
#include "Pixel2DAnimNotifyEvent.h"

bool operator==(const FPixel2DAnimNotifyEventReference& Lhs, const FPixel2DAnimNotifyEvent& Rhs)
{
	if (Lhs.NotifySource && Lhs.Notify)
	{
		return (*(Lhs.Notify)) == Rhs;
	}
	return false;
}

void FPixel2DAnimNotifyQueue::AddAnimNotifiesToDestNoFiltering(const TArray<FPixel2DAnimNotifyEventReference>& NewNotifies, TArray<FPixel2DAnimNotifyEventReference>& DestArray) const
{
	for (const FPixel2DAnimNotifyEventReference& NotifyRef : NewNotifies)
	{
		if (const FPixel2DAnimNotifyEvent* Notify = NotifyRef.GetNotify())
		{
			Notify->NotifyStateClass ? DestArray.AddUnique(NotifyRef) : DestArray.Add(NotifyRef);
		}
	}
}

void FPixel2DAnimNotifyQueue::AddAnimNotify(const FPixel2DAnimNotifyEvent* Notify, const UObject* NotifySource)
{
	AnimNotifies.Add(FPixel2DAnimNotifyEventReference(Notify, NotifySource));
}

void FPixel2DAnimNotifyQueue::Reset()
{
	AnimNotifies.Reset();
}

void FPixel2DAnimNotifyQueue::Append(const FPixel2DAnimNotifyQueue& Queue)
{
	// we dont just append here - we need to preserve uniqueness for AnimNotifyState instances
	AddAnimNotifiesToDestNoFiltering(Queue.AnimNotifies, AnimNotifies);
}
