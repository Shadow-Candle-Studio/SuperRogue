// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KismetCompiler.h"
#include "Pixel2DAnimNode_Base.h"
#include "Pixel2DAnimGraphNode_Base.h"
#include "KismetCompilerModule.h"
#include "Pixel2DAnimGraphSchema.h"
#include "UK2Node_FlipbookGetter.h"

struct FPixel2DPoseLinkMappingRecord;
class UPixel2DAnimBlueprint;
class UPixel2DAnimBlueprintGeneratedClass;
class UPixel2DAnimStateTransitionNode;
class UPixel2DAnimGraphNode_StateMachineBase;
class UPixel2DAnimGraphNode_StateResult;

//////////////////////////////////////////////////////////////////////////
// FPixel2DAnimBlueprintCompilerContext

class FPixel2DAnimBlueprintCompilerContext : public FKismetCompilerContext
{
protected:
	typedef FKismetCompilerContext Super;
public:
	FPixel2DAnimBlueprintCompilerContext(UPixel2DAnimBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompileOptions);
	virtual ~FPixel2DAnimBlueprintCompilerContext();

	virtual void PostCompile() override;

protected:
	// Implementation of FKismetCompilerContext interface
	virtual void CreateClassVariablesFromBlueprint() override;
	virtual UEdGraphSchema_K2* CreateSchema() override;
	virtual void MergeUbergraphPagesIn(UEdGraph* Ubergraph) override;
	virtual void ProcessOneFunctionGraph(UEdGraph* SourceGraph, bool bInternalFunction = false) override;
	virtual void CreateFunctionList() override;
	virtual void SpawnNewClass(const FString& NewClassName) override;
	virtual void OnNewClassSet(UBlueprintGeneratedClass* ClassToUse) override;
	virtual void CopyTermDefaultsToDefaultObject(UObject* DefaultObject) override;
	virtual void PostCompileDiagnostics() override;
	virtual void EnsureProperGeneratedClass(UClass*& TargetClass) override;
	virtual void CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOldCDO) override;
	virtual void FinishCompilingClass(UClass* Class) override;
	// End of FKismetCompilerContext interface

protected:
	typedef TArray<UEdGraphPin*> UEdGraphPinArray;
	
	protected:
		/** Record of a single copy operation */
		struct FPropertyCopyRecord
		{
			FPropertyCopyRecord(UEdGraphPin* InDestPin, FProperty* InDestProperty, int32 InDestArrayIndex)
				: DestPin(InDestPin)
				, DestProperty(InDestProperty)
				, DestArrayIndex(InDestArrayIndex)
				, SourcePropertyName(NAME_None)
				, SourceSubStructPropertyName(NAME_None)
				, Operation(EPostCopyOperation::None)
			{}
	
			bool IsFastPath() const
			{
				return DestProperty != nullptr && SourcePropertyName != NAME_None;
			}
	
			void InvalidateFastPath()
			{
				SourcePropertyName = NAME_None;
				SourceSubStructPropertyName = NAME_None;
			}
	
			void ValidateFastPath(UClass* InCompiledClass);
	
			/** The destination pin we are copying to */
			UEdGraphPin* DestPin;
	
			/** The destination property we are copying to (on an animation node) */
			FProperty* DestProperty;
	
			/** The array index we use if the destination property is an array */
			int32 DestArrayIndex;
	
			/** The source property we are copying from (on an anim instance) */
			FName SourcePropertyName;
	
			/** The source sub-struct property we are copying from (if the source property is a struct property) */
			FName SourceSubStructPropertyName;
	
			/** Any operation we want to perform post-copy on the destination data */
			EPostCopyOperation Operation;
		};
	
		// Wireup record for a single anim node property (which might be an array)
		struct FAnimNodeSinglePropertyHandler
		{
			/** Copy records */
			TArray<FPropertyCopyRecord> CopyRecords;
	
			// If the anim instance is the container target instead of the node.
			bool bInstanceIsTarget;
	
			FAnimNodeSinglePropertyHandler()
				: bInstanceIsTarget(false)
			{
			}
		};
	
		// Record for a property that was exposed as a pin, but wasn't wired up (just a literal)
		struct FEffectiveConstantRecord
		{
		public:
			// The node variable that the handler is in
			class FStructProperty* NodeVariableProperty;
	
			// The property within the struct to set
			class FProperty* ConstantProperty;
	
			// The array index if ConstantProperty is an array property, or INDEX_NONE otherwise
			int32 ArrayIndex;
	
			// The pin to pull the DefaultValue/DefaultObject from
			UEdGraphPin* LiteralSourcePin;
	
			FEffectiveConstantRecord()
				: NodeVariableProperty(NULL)
				, ConstantProperty(NULL)
				, ArrayIndex(INDEX_NONE)
				, LiteralSourcePin(NULL)
			{
			}
	
			FEffectiveConstantRecord(FStructProperty* ContainingNodeProperty, UEdGraphPin* SourcePin, FProperty* SourcePinProperty, int32 SourceArrayIndex)
				: NodeVariableProperty(ContainingNodeProperty)
				, ConstantProperty(SourcePinProperty)
				, ArrayIndex(SourceArrayIndex)
				, LiteralSourcePin(SourcePin)
			{
			}
	
			bool Apply(UObject* Object);
		};
	
		struct FEvaluationHandlerRecord
		{
		public:
	
			// The node variable that the handler is in
			FStructProperty* NodeVariableProperty;
	
			// The specific evaluation handler inside the specified node
			FStructProperty* EvaluationHandlerProperty;
	
			// Whether or not our serviced properties are actually on the instance instead of the node
			bool bServicesInstanceProperties;
	
			// Set of properties serviced by this handler (Map from property name to the record for that property)
			TMap<FName, FAnimNodeSinglePropertyHandler> ServicedProperties;
	
			// The resulting function name
			FName HandlerFunctionName;
	
		public:
	
			FEvaluationHandlerRecord()
				: NodeVariableProperty(nullptr)
				, EvaluationHandlerProperty(nullptr)
				, bServicesInstanceProperties(false)
				, HandlerFunctionName(NAME_None)
			{}
	
			bool IsFastPath() const
			{
				for (TMap<FName, FAnimNodeSinglePropertyHandler>::TConstIterator It(ServicedProperties); It; ++It)
				{
					const FAnimNodeSinglePropertyHandler& AnimNodeSinglePropertyHandler = It.Value();
					for (const FPropertyCopyRecord& CopyRecord : AnimNodeSinglePropertyHandler.CopyRecords)
					{
						if (!CopyRecord.IsFastPath())
						{
							return false;
						}
					}
				}
	
				return true;
			}
	
			bool IsValid() const
			{
				return NodeVariableProperty != nullptr && EvaluationHandlerProperty != nullptr;
			}
	
			void PatchFunctionNameAndCopyRecordsInto(UObject* TargetObject) const;
	
			void RegisterPin(UEdGraphPin* DestPin, FProperty* AssociatedProperty, int32 AssociatedPropertyArrayIndex);
	
			FStructProperty* GetHandlerNodeProperty() const { return NodeVariableProperty; }
	
			void BuildFastPathCopyRecords();
	
			void ValidateFastPath(UClass* InCompiledClass);
	
		private:
	
			bool CheckForVariableGet(FPropertyCopyRecord& CopyRecord, UEdGraphPin* DestPin);
	
			bool CheckForLogicalNot(FPropertyCopyRecord& CopyRecord, UEdGraphPin* DestPin);
	
			bool CheckForStructMemberAccess(FPropertyCopyRecord& CopyRecord, UEdGraphPin* DestPin);
	
			bool CheckForMemberOnlyAccess(FPropertyCopyRecord& CopyRecord, UEdGraphPin* DestPin);
		};
	
		// State machines may get processed before their inner graphs, so their node index needs to be patched up later
		// This structure records pending fixups.
		struct FStateRootNodeIndexFixup
		{
		public:
			int32 MachineIndex;
			int32 StateIndex;
			UPixel2DAnimGraphNode_StateResult* ResultNode;
	
		public:
			FStateRootNodeIndexFixup(int32 InMachineIndex, int32 InStateIndex, UPixel2DAnimGraphNode_StateResult* InResultNode)
				: MachineIndex(InMachineIndex)
				, StateIndex(InStateIndex)
				, ResultNode(InResultNode)
			{
			}
		};
	
	protected:
		UPixel2DAnimBlueprintGeneratedClass* NewSpriteAnimBlueprintClass;
		UPixel2DAnimBlueprint* AnimBlueprint;
	
		UPixel2DAnimGraphSchema* SpriteAnimSchema;
	
		// Map of allocated v3 nodes that are members of the class
		TMap<class UPixel2DAnimGraphNode_Base*, FProperty*> AllocatedAnimNodes;
		TMap<FProperty*, class UPixel2DAnimGraphNode_Base*> AllocatedNodePropertiesToNodes;
		TMap<int32, FProperty*> AllocatedPropertiesByIndex;
	
		// Map of true source objects (user edited ones) to the cloned ones that are actually compiled
		TMap<class UPixel2DAnimGraphNode_Base*, UPixel2DAnimGraphNode_Base*> SourceNodeToProcessedNodeMap;
	
		// Index of the nodes (must match up with the runtime discovery process of nodes, which runs thru the property chain)
		int32 AllocateNodeIndexCounter;
		TMap<class UPixel2DAnimGraphNode_Base*, int32> AllocatedAnimNodeIndices;
	
		// Map from pose link LinkID address
		//@TODO: Bad structure for a list of these
		TArray<FPixel2DPoseLinkMappingRecord> ValidPoseLinkList;
	
		// List of successfully created evaluation handlers
		TArray<FEvaluationHandlerRecord> ValidEvaluationHandlerList;
	
		// List of animation node literals (values exposed as pins but never wired up) that need to be pushed into the CDO
		TArray<FEffectiveConstantRecord> ValidAnimNodePinConstants;

		// List of getter node's we've found so the auto-wire can be deferred till after state machine compilation
		TArray<class UK2Node_FlipbookGetter*> FoundGetterNodes;

		// Set of used handler function names
		TSet<FName> HandlerFunctionNames;

		// True if any parent class is also generated from an animation blueprint
		bool bIsDerivedAnimBlueprint;
	private:

		// Creates an evaluation handler for an FExposedValue property in an animation node
		void CreateEvaluationHandlerStruct(UPixel2DAnimGraphNode_Base* VisualAnimNode, FEvaluationHandlerRecord& Record);
		void CreateEvaluationHandlerInstance(UPixel2DAnimGraphNode_Base* VisualAnimNode, FEvaluationHandlerRecord& Record);
	
		// Prunes any nodes that aren't reachable via a pose link
		void PruneIsolatedAnimationNodes(const TArray<UPixel2DAnimGraphNode_Base*>& RootSet, TArray<UPixel2DAnimGraphNode_Base*>& GraphNodes);
	
		// Compiles one animation node
		void ProcessAnimationNode(UPixel2DAnimGraphNode_Base* VisualAnimNode);
	
		// Compiles one state machine
		void ProcessStateMachine(UPixel2DAnimGraphNode_StateMachineBase* StateMachineInstance);
	
		// Compiles an entire animation graph
		void ProcessAllAnimationNodes();
	
		//
		void ProcessAnimationNodesGivenRoot(TArray<UPixel2DAnimGraphNode_Base*>& AnimNodeList, const TArray<UPixel2DAnimGraphNode_Base*>& RootSet);

		// Automatically fill in parameters for the specified Getter node
		void AutoWireFlipbookGetter(class UK2Node_FlipbookGetter* Getter, UPixel2DAnimStateTransitionNode* InTransitionNode);

		// This function does the following steps:
		//   Clones the nodes in the specified source graph
		//   Merges them into the ConsolidatedEventGraph
		//   Processes any animation nodes
		//   Returns the index of the processed cloned version of SourceRootNode
		//	 If supplied, will also return an array of all cloned nodes
		int32 ExpandGraphAndProcessNodes(UEdGraph* SourceGraph, UPixel2DAnimGraphNode_Base* SourceRootNode, UPixel2DAnimStateTransitionNode* TransitionNode = NULL, TArray<UEdGraphNode*>* ClonedNodes = NULL);

		// Returns the allocation index of the specified node, processing it if it was pending
		int32 GetAllocationIndexOfNode(UPixel2DAnimGraphNode_Base* VisualAnimNode);
};
