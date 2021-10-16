// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNode_Base.h"
#include "Pixel2DAnimInstanceProxy.h"

/////////////////////////////////////////////////////
// FPixel2DAnimationBaseContext

FPixel2DAnimationBaseContext::FPixel2DAnimationBaseContext(FPixel2DAnimInstanceProxy* InAnimInstanceProxy)
	: AnimInstanceProxy(InAnimInstanceProxy)
{
}

UPixel2DAnimBlueprintGeneratedClass* FPixel2DAnimationBaseContext::GetAnimClass() const
{
	return AnimInstanceProxy ? AnimInstanceProxy->GetAnimClassInterface() : nullptr;
}

/////////////////////////////////////////////////////
// FPixel2DPoseContext

void FPixel2DPoseContext::Initialize(FPixel2DAnimInstanceProxy* InAnimInstanceProxy)
{
	Flipbook = NULL;
}


void FPixel2DAnimNode_Base::Initialize_AnyThread(const FPixel2DAnimationInitializeContext& Context)
{
}

void FPixel2DAnimNode_Base::Update_AnyThread(const FPixel2DAnimationUpdateContext& Context)
{
}

void FPixel2DAnimNode_Base::Evaluate_AnyThread(FPixel2DPoseContext& Output)
{
}

/////////////////////////////////////////////////////
// FPixel2DPoseLinkBase

void FPixel2DPoseLinkBase::AttemptRelink(const FPixel2DAnimationBaseContext& Context)
{
	// Do the linkage
	if ((LinkedNode == NULL) && (LinkID != INDEX_NONE))
	{
		UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintClass = Context.GetAnimClass();
		check(AnimBlueprintClass);

		// adding ensure. We had a crash on here
		if (ensure(AnimBlueprintClass->GetAnimNodeProperties().IsValidIndex(LinkID)))
		{
			FProperty* LinkedProperty = AnimBlueprintClass->GetAnimNodeProperties()[LinkID];
			void* LinkedNodePtr = LinkedProperty->ContainerPtrToValuePtr<void>(Context.AnimInstanceProxy->GetAnimInstanceObject());
			LinkedNode = (FPixel2DAnimNode_Base*)LinkedNodePtr;
		}
	}
}

void FPixel2DPoseLinkBase::Initialize(const FPixel2DAnimationInitializeContext& Context)
{
	AttemptRelink(Context);

	// Do standard initialization
	if (LinkedNode != NULL)
	{
		LinkedNode->Initialize_AnyThread(Context);
	}
}


void FPixel2DPoseLinkBase::Update(const FPixel2DAnimationUpdateContext& Context)
{

#if WITH_EDITOR
	if (GIsEditor)
	{
		if (LinkedNode == NULL)
		{
			//@TODO: Should only do this when playing back
			AttemptRelink(Context);
		}
	}
#endif

	if (LinkedNode != NULL)
	{
		LinkedNode->Update_AnyThread(Context);
	}
}

/////////////////////////////////////////////////////
// FPixel2DPoseLink

void FPixel2DPoseLink::Evaluate(FPixel2DPoseContext& Output)
{

#if WITH_EDITOR
	if ((LinkedNode == NULL) && GIsEditor)
	{
		//@TODO: Should only do this when playing back
		AttemptRelink(Output);
	}
#endif


	if (LinkedNode != NULL)
	{
		LinkedNode->Evaluate_AnyThread(Output);
	}
	else
	{
		//@TODO: Warning here?
	}
}

void FPaper2DExposedValueHandler::Initialize(FPixel2DAnimNode_Base* AnimNode, UObject* AnimInstanceObject)
{
	if (bInitialized)
	{
		return;
	}

	if (BoundFunction != NAME_None)
	{
		// we cant call FindFunction on anything but the game thread as it accesses a shared map in the object's class
		check(IsInGameThread());
		Function = AnimInstanceObject->FindFunction(BoundFunction);
		check(Function);
	}
	else
	{
		Function = NULL;
	}

	// initialize copy records
	for (FPixel2DExposedValueCopyRecord& CopyRecord : CopyRecords)
	{
		FProperty* SourceProperty = AnimInstanceObject->GetClass()->FindPropertyByName(CopyRecord.SourcePropertyName);
		check(SourceProperty);
		if (FArrayProperty* SourceArrayProperty = CastField<FArrayProperty>(SourceProperty))
		{
			// the compiler should not be generating any code that calls down this path at the moment - it is untested
			check(false);
		}
		else
		{
			if (CopyRecord.SourceSubPropertyName != NAME_None)
			{
				void* Source = SourceProperty->ContainerPtrToValuePtr<uint8>(AnimInstanceObject, 0);
				FStructProperty* SourceStructProperty = CastFieldChecked<FStructProperty>(SourceProperty);
				FProperty* SourceStructSubProperty = SourceStructProperty->Struct->FindPropertyByName(CopyRecord.SourceSubPropertyName);
				CopyRecord.Source = SourceStructSubProperty->ContainerPtrToValuePtr<uint8>(Source, CopyRecord.SourceArrayIndex);
				CopyRecord.Size = SourceStructSubProperty->GetSize();
				CopyRecord.CachedSourceProperty = SourceStructSubProperty;
				CopyRecord.CachedSourceContainer = Source;
			}
			else
			{
				CopyRecord.Source = SourceProperty->ContainerPtrToValuePtr<uint8>(AnimInstanceObject, CopyRecord.SourceArrayIndex);
				CopyRecord.Size = SourceProperty->GetSize();
				CopyRecord.CachedSourceProperty = SourceProperty;
				CopyRecord.CachedSourceContainer = AnimInstanceObject;
			}
		}

		if (FArrayProperty* DestArrayProperty = CastField<FArrayProperty>(CopyRecord.DestProperty.Get()))
		{
			FScriptArrayHelper ArrayHelper(DestArrayProperty, CopyRecord.DestProperty->ContainerPtrToValuePtr<uint8>(AnimNode));
			check(ArrayHelper.IsValidIndex(CopyRecord.DestArrayIndex));
			CopyRecord.Dest = ArrayHelper.GetRawPtr(CopyRecord.DestArrayIndex);

			if (CopyRecord.bInstanceIsTarget)
			{
				CopyRecord.CachedDestContainer = AnimInstanceObject;
			}
			else
			{
				CopyRecord.CachedDestContainer = AnimNode;
			}
		}
		else
		{
			CopyRecord.Dest = CopyRecord.DestProperty->ContainerPtrToValuePtr<uint8>(AnimNode, CopyRecord.DestArrayIndex);

			if (CopyRecord.bInstanceIsTarget)
			{
				CopyRecord.CachedDestContainer = AnimInstanceObject;
				CopyRecord.Dest = CopyRecord.DestProperty->ContainerPtrToValuePtr<uint8>(AnimInstanceObject, CopyRecord.DestArrayIndex);
			}
			else
			{
				CopyRecord.CachedDestContainer = AnimNode;
			}
		}

		if (FNameProperty* NameProperty = CastField<FNameProperty>(CopyRecord.DestProperty.Get()))
		{
			CopyRecord.CopyType = ECopyType::NameProperty;
		}
		if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(CopyRecord.DestProperty.Get()))
		{
			CopyRecord.CopyType = ECopyType::BoolProperty;
		}
		else if (FStructProperty* StructProperty = CastField<FStructProperty>(CopyRecord.DestProperty.Get()))
		{
			CopyRecord.CopyType = ECopyType::StructProperty;
		}
		else if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(CopyRecord.DestProperty.Get()))
		{
			CopyRecord.CopyType = ECopyType::ObjectProperty;
		}
		else
		{
			CopyRecord.CopyType = ECopyType::PlainProperty;
		}
	}

	bInitialized = true;
}

void FPaper2DExposedValueHandler::Execute(const FPixel2DAnimationBaseContext& Context) const
{
	if (Function != nullptr)
	{
		Context.AnimInstanceProxy->GetAnimInstanceObject()->ProcessEvent(Function, NULL);
	}

	for (const FPixel2DExposedValueCopyRecord& CopyRecord : CopyRecords)
	{
		// if any of these checks fail then it's likely that Initialize has not been called.
		// has new anim node type been added that doesnt call the base class Initialize()?
		checkSlow(CopyRecord.Dest != nullptr);
		checkSlow(CopyRecord.Source != nullptr);
		checkSlow(CopyRecord.Size != 0);

		switch (CopyRecord.PostCopyOperation)
		{
		case EPostCopyOperation::None:
		{
			switch (CopyRecord.CopyType)
			{
			default:
			case ECopyType::PlainProperty:
				FMemory::Memcpy(CopyRecord.Dest, CopyRecord.Source, CopyRecord.Size);
				break;
			case ECopyType::BoolProperty:
			{
				bool bValue = static_cast<FBoolProperty*>(CopyRecord.CachedSourceProperty.Get())->GetPropertyValue_InContainer(CopyRecord.CachedSourceContainer);
				static_cast<FBoolProperty*>(CopyRecord.DestProperty.Get())->SetPropertyValue_InContainer(CopyRecord.CachedDestContainer, bValue, CopyRecord.DestArrayIndex);
			}
			break;
			case ECopyType::StructProperty:
				static_cast<FStructProperty*>(CopyRecord.DestProperty.Get())->Struct->CopyScriptStruct(CopyRecord.Dest, CopyRecord.Source);
				break;
			case ECopyType::ObjectProperty:
			{
				UObject* Value = static_cast<FObjectPropertyBase*>(CopyRecord.CachedSourceProperty.Get())->GetObjectPropertyValue_InContainer(CopyRecord.CachedSourceContainer);
				static_cast<FObjectPropertyBase*>(CopyRecord.DestProperty.Get())->SetObjectPropertyValue_InContainer(CopyRecord.CachedDestContainer, Value, CopyRecord.DestArrayIndex);
			}
			case ECopyType::NameProperty:
			{
				const FName& NameValue = static_cast<FNameProperty*>(CopyRecord.CachedSourceProperty.Get())->GetPropertyValue_InContainer(CopyRecord.CachedSourceContainer);
				static_cast<FNameProperty*>(CopyRecord.DestProperty.Get())->SetPropertyValue_InContainer(CopyRecord.CachedDestContainer, NameValue);
			}
			break;
			}
		}
		break;
		case EPostCopyOperation::LogicalNegateBool:
		{
			check(CopyRecord.CachedSourceProperty != nullptr && CopyRecord.DestProperty != nullptr);

			bool bValue = static_cast<FBoolProperty*>(CopyRecord.CachedSourceProperty.Get())->GetPropertyValue_InContainer(CopyRecord.CachedSourceContainer);
			static_cast<FBoolProperty*>(CopyRecord.DestProperty.Get())->SetPropertyValue_InContainer(CopyRecord.CachedDestContainer, !bValue, CopyRecord.DestArrayIndex);
		}
		break;
		}
	}
}
