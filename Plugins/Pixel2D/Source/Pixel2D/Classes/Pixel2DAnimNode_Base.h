// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "Animation/AnimNodeBase.h"
#include "PaperFlipbook.h"
#include "Pixel2DAnimNode_Base.generated.h"

class UPixel2DAnimBlueprint;
class UPixel2DAnimInstance;
class UPixel2DAnimBlueprintGeneratedClass;
struct FPixel2DAnimInstanceProxy;
struct FPixel2DAnimNode_Base;
struct FPixel2DAnimNotifyQueue;

/** Base class for update/evaluate contexts */
struct FPixel2DAnimationBaseContext
{
public:
	FPixel2DAnimInstanceProxy* AnimInstanceProxy;

protected:
	PIXEL2D_API FPixel2DAnimationBaseContext(FPixel2DAnimInstanceProxy* InAnimInstanceProxy);

public:
	// Get the Blueprint IAnimClassInterface associated with this context, if there is one.
	// Note: This can return NULL, so check the result.
	PIXEL2D_API UPixel2DAnimBlueprintGeneratedClass* GetAnimClass() const;
};

/** Initialization context passed around during animation tree initialization */
struct FPixel2DAnimationInitializeContext : public FPixel2DAnimationBaseContext
{
public:
	FPixel2DAnimationInitializeContext(FPixel2DAnimInstanceProxy* InAnimInstanceProxy)
		: FPixel2DAnimationBaseContext(InAnimInstanceProxy)
	{
	}
};

struct FPixel2DAnimationUpdateContext : public FPixel2DAnimationBaseContext
{
	float DeltaTime;
	FPixel2DAnimNotifyQueue *NotifyQueue;
public:
	FPixel2DAnimationUpdateContext(FPixel2DAnimInstanceProxy* InAnimInstanceProxy, float InDeltaTime, FPixel2DAnimNotifyQueue * NotifyQueue)
		: FPixel2DAnimationBaseContext(InAnimInstanceProxy)
		, DeltaTime(InDeltaTime)
		, NotifyQueue(NotifyQueue)
	{
	}

	// Returns the delta time for this update, in seconds
	float GetDeltaTime() const { return DeltaTime; }
};


/** Evaluation context passed around during animation tree evaluation */
struct FPixel2DPoseContext : public FPixel2DAnimationBaseContext
{
public:
	UPaperFlipbook *Flipbook;

public:
	// This constructor allocates a new uninitialized pose for the specified anim instance
	FPixel2DPoseContext(FPixel2DAnimInstanceProxy* InAnimInstanceProxy)
		: FPixel2DAnimationBaseContext(InAnimInstanceProxy)
	{
		Initialize(InAnimInstanceProxy);
	}

	// This constructor allocates a new uninitialized pose, copying non-pose state from the source context
	FPixel2DPoseContext(const FPixel2DPoseContext& SourceContext)
		: FPixel2DAnimationBaseContext(SourceContext.AnimInstanceProxy)
	{
		Initialize(SourceContext.AnimInstanceProxy);
	}

	PIXEL2D_API void Initialize(FPixel2DAnimInstanceProxy* InAnimInstanceProxy);
};


/** A pose link to another node */
USTRUCT(BlueprintInternalUseOnly)
struct PIXEL2D_API FPixel2DPoseLinkBase
{
	GENERATED_USTRUCT_BODY()

	/** Serialized link ID, used to build the non-serialized pointer map. */
	UPROPERTY()
	int32 LinkID;

#if WITH_EDITORONLY_DATA
	/** The source link ID, used for debug visualization. */
	UPROPERTY()
	int32 SourceLinkID;
#endif

protected:
	/** The non serialized node pointer. */
	struct FPixel2DAnimNode_Base* LinkedNode;

	/** Flag to prevent reentry when dealing with circular trees. */
	bool bProcessed;

public:
	FPixel2DPoseLinkBase()
		: LinkID(INDEX_NONE)
#if WITH_EDITORONLY_DATA
		, SourceLinkID(INDEX_NONE)
#endif
		, LinkedNode(NULL)
		, bProcessed(false)
	{
	}

	// Interface
	void Initialize(const FPixel2DAnimationInitializeContext& Context);
	void Update(const FPixel2DAnimationUpdateContext& Context);

	/** Try to re-establish the linked node pointer. */
	void AttemptRelink(const FPixel2DAnimationBaseContext& Context);

};

USTRUCT(BlueprintInternalUseOnly)
struct PIXEL2D_API FPixel2DPoseLink : public FPixel2DPoseLinkBase
{
	GENERATED_USTRUCT_BODY()

public:
	// Interface
	void Evaluate(FPixel2DPoseContext& Output);
};

UENUM()
enum class ECopyType : uint8
{
	// For plain old data types, we do a simple memcpy.
	PlainProperty,

	// Read and write properties using bool property helpers, as source/dest could be bitfield or boolean
	BoolProperty,

	// Use struct copy operation, as this needs to correctly handle CPP struct ops
	StructProperty,

	// Read and write properties using object property helpers, as source/dest could be regular/weak/lazy etc.
	ObjectProperty,

	// FName needs special case because its size changes between editor/compiler and runtime.
	NameProperty,
};



USTRUCT()
struct FPixel2DExposedValueCopyRecord
{
    GENERATED_USTRUCT_BODY()

		FPixel2DExposedValueCopyRecord()
        : SourcePropertyName(NAME_None)
        , SourceSubPropertyName(NAME_None)
        , SourceArrayIndex(0)
        , DestProperty(nullptr)
        , DestArrayIndex(0)
        , Size(0)
        , bInstanceIsTarget(false)
        , PostCopyOperation(EPostCopyOperation::None)
        , CopyType(ECopyType::PlainProperty)
        , CachedSourceProperty(nullptr)
        , CachedSourceContainer(nullptr)
        , CachedDestContainer(nullptr)
        , Source(nullptr)
        , Dest(nullptr)
    {}

    UPROPERTY()
    FName SourcePropertyName;

    UPROPERTY()
    FName SourceSubPropertyName;

    UPROPERTY()
    int32 SourceArrayIndex;

    UPROPERTY()
	TFieldPath<FProperty> DestProperty;

    UPROPERTY()
    int32 DestArrayIndex;

    UPROPERTY()
    int32 Size;

   // Whether or not the anim instance object is the target for the copy instead of a node.
    UPROPERTY()
    bool bInstanceIsTarget;

    UPROPERTY()
    EPostCopyOperation PostCopyOperation;

    UPROPERTY(Transient)
    ECopyType CopyType;

    // cached source property
    UPROPERTY(Transient)
	TFieldPath<FProperty> CachedSourceProperty;

    // cached source container for use with boolean operations
    void* CachedSourceContainer;

    // cached dest container for use with boolean operations
    void* CachedDestContainer;

    // Cached source copy ptr
    void* Source;

    // Cached dest copy ptr
    void* Dest;
};


// An exposed value updater
USTRUCT()
struct PIXEL2D_API FPaper2DExposedValueHandler
{
	GENERATED_USTRUCT_BODY()

	FPaper2DExposedValueHandler()
        : BoundFunction(NAME_None)
        , Function(nullptr)
        , bInitialized(false)
	{
	}

    // The function to call to update associated properties (can be NULL)
    UPROPERTY()
    FName BoundFunction;

    // Direct data access to property in anim instance
    UPROPERTY()
    TArray<FPixel2DExposedValueCopyRecord> CopyRecords;

    // function pointer if BoundFunction != NAME_None
    UFunction* Function;

    // Prevent multiple initialization
    bool bInitialized;

	// Bind copy records and cache UFunction if necessary
	void Initialize(FPixel2DAnimNode_Base* AnimNode, UObject* AnimInstanceObject);

	// Execute the function and copy records
	void Execute(const FPixel2DAnimationBaseContext& Context) const;
};

/**
* This is the base of all runtime animation nodes
*
* To create a new animation node:
*   Create a struct derived from FPixel2DAnimNode_Base - this is your runtime node
*   Create a class derived from UPixel2DAnimGraphNode_Base, containing an instance of your runtime node as a member - this is your visual/editor-only node
*/
USTRUCT()
struct PIXEL2D_API FPixel2DAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	// The default handler for graph-exposed inputs
	UPROPERTY(meta = (BlueprintCompilerGeneratedDefaults))
	FPaper2DExposedValueHandler EvaluateGraphExposedInputs;

	/**
	* Called when the node first runs. If the node is inside a state machine or cached pose branch then this can be called multiple times.
	* This can be called on any thread.
	* @param	Context		Context structure providing access to relevant data
	*/
	virtual void Initialize_AnyThread(const FPixel2DAnimationInitializeContext& Context);

	/**
	* Called to update the state of the graph relative to this node.
	* Generally this should configure any weights (etc.) that could affect the poses that
	* will need to be evaluated. This function is what usually executes EvaluateGraphExposedInputs.
	* This can be called on any thread.
	* @param	Context		Context structure providing access to relevant data
	*/
	virtual void Update_AnyThread(const FPixel2DAnimationUpdateContext& Context);

	virtual void Evaluate_AnyThread(FPixel2DPoseContext& Output);

    /** Called after compilation */
	virtual void PostCompile() {}

	virtual ~FPixel2DAnimNode_Base() {}
};