// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimBlueprintCompilerContext.h"
#include "UObject/UObjectHash.h"
#include "Pixel2DAnimInstance.h"
#include "EdGraphUtilities.h"
#include "K2Node_CallFunction.h"
#include "K2Node_StructMemberGet.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_CallArrayFunction.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_Knot.h"
#include "K2Node_StructMemberSet.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"

#include "Pixel2DAnimBlueprintGeneratedClass.h"

#include "Pixel2DAnimGraphSchema.h"
#include "K2Node_TransitionRuleGetter.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetReinstanceUtilities.h"
#include "Pixel2DAnimStateNodeBase.h"
#include "Pixel2DAnimStateNode.h"
#include "Pixel2DAnimStateEntryNode.h"
#include "Pixel2DAnimStateTransitionNode.h"
#include "Pixel2DAnimStateGraph.h"
#include "Pixel2DAnimStateMachineGraph.h"
#include "Pixel2DAnimStateMachineSchema.h"
#include "Pixel2DAnimTransitionGraph.h"
#include "Pixel2DAnimGraphNode_Root.h"
#include "Pixel2DAnimGraphNode_StateMachineBase.h"
#include "Pixel2DAnimGraphNode_TransitionResult.h"
#include "K2Node_AnimGetter.h"
#include "Pixel2DAnimGraphNode_StateMachine.h"
#include "Pixel2DAnimGraphNode_AssetPlayerBase.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnimBlueprintCompiler"

////////////////////////////////////////////////////////////////////////
// FPixel2DAnimBlueprintCompilerContext::FEffectiveConstantRecord

bool FPixel2DAnimBlueprintCompilerContext::FEffectiveConstantRecord::Apply(UObject* Object)
{
	uint8* StructPtr = nullptr;
	uint8* PropertyPtr = nullptr;

	{
		StructPtr = NodeVariableProperty->ContainerPtrToValuePtr<uint8>(Object);
		PropertyPtr = ConstantProperty->ContainerPtrToValuePtr<uint8>(StructPtr);
	}

	if (ArrayIndex != INDEX_NONE)
	{
		FArrayProperty* ArrayProperty = CastFieldChecked<FArrayProperty>(ConstantProperty);

		// Peer inside the array
		FScriptArrayHelper ArrayHelper(ArrayProperty, PropertyPtr);

		if (ArrayHelper.IsValidIndex(ArrayIndex))
		{
			FBlueprintEditorUtils::PropertyValueFromString_Direct(ArrayProperty->Inner, LiteralSourcePin->GetDefaultAsString(), ArrayHelper.GetRawPtr(ArrayIndex));
		}
		else
		{
			return false;
		}
	}
	else
	{
		FBlueprintEditorUtils::PropertyValueFromString_Direct(ConstantProperty, LiteralSourcePin->GetDefaultAsString(), PropertyPtr);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// FPixel2DAnimBlueprintCompilerContext

FPixel2DAnimBlueprintCompilerContext::FPixel2DAnimBlueprintCompilerContext
(UPixel2DAnimBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog,
	const FKismetCompilerOptions& InCompileOptions)
	: FKismetCompilerContext(SourceSketch, InMessageLog, InCompileOptions)
	, AnimBlueprint(SourceSketch)
	, bIsDerivedAnimBlueprint(false)
{
}

FPixel2DAnimBlueprintCompilerContext::~FPixel2DAnimBlueprintCompilerContext()
{
}

void FPixel2DAnimBlueprintCompilerContext::CreateClassVariablesFromBlueprint()
{
	FKismetCompilerContext::CreateClassVariablesFromBlueprint();
}


UEdGraphSchema_K2* FPixel2DAnimBlueprintCompilerContext::CreateSchema()
{
	SpriteAnimSchema = NewObject<UPixel2DAnimGraphSchema>();
	return SpriteAnimSchema;
}

void FPixel2DAnimBlueprintCompilerContext::CreateEvaluationHandlerStruct(UPixel2DAnimGraphNode_Base* VisualAnimNode, FEvaluationHandlerRecord& Record)
{
	// Shouldn't create a handler if there is nothing to work with
	check(Record.ServicedProperties.Num() > 0);
	check(Record.NodeVariableProperty != NULL);

	// Use the node GUID for a stable name across compiles
	FString FunctionName = FString::Printf(TEXT("%s_%s_%s_%s"), *Record.EvaluationHandlerProperty->GetName(), *VisualAnimNode->GetOuter()->GetName(), *VisualAnimNode->GetClass()->GetName(), *VisualAnimNode->NodeGuid.ToString());
	Record.HandlerFunctionName = FName(*FunctionName);

	// check function name isnt already used (data exists that can contain duplicate GUIDs) and apply a numeric extension until it is unique
	int32 ExtensionIndex = 0;
	FName* ExistingName = HandlerFunctionNames.Find(Record.HandlerFunctionName);
	while (ExistingName != nullptr)
	{
		FunctionName = FString::Printf(TEXT("%s_%s_%s_%s_%d"), *Record.EvaluationHandlerProperty->GetName(), *VisualAnimNode->GetOuter()->GetName(), *VisualAnimNode->GetClass()->GetName(), *VisualAnimNode->NodeGuid.ToString(), ExtensionIndex);
		Record.HandlerFunctionName = FName(*FunctionName);
		ExistingName = HandlerFunctionNames.Find(Record.HandlerFunctionName);
		ExtensionIndex++;
	}

	HandlerFunctionNames.Add(Record.HandlerFunctionName);

	// Add a custom event in the graph
	UK2Node_CustomEvent* EntryNode = SpawnIntermediateEventNode<UK2Node_CustomEvent>(VisualAnimNode, nullptr, ConsolidatedEventGraph);
	EntryNode->bInternalEvent = true;
	EntryNode->CustomFunctionName = Record.HandlerFunctionName;
	EntryNode->AllocateDefaultPins();

	// The ExecChain is the current exec output pin in the linear chain
	UEdGraphPin* ExecChain = Schema->FindExecutionPin(*EntryNode, EGPD_Output);

	// Create a struct member write node to store the parameters into the animation node
	UK2Node_StructMemberSet* AssignmentNode = SpawnIntermediateNode<UK2Node_StructMemberSet>(VisualAnimNode, ConsolidatedEventGraph);
	AssignmentNode->VariableReference.SetSelfMember(Record.NodeVariableProperty->GetFName());
	AssignmentNode->StructType = Record.NodeVariableProperty->Struct;
	AssignmentNode->AllocateDefaultPins();

	// Wire up the variable node execution wires
	UEdGraphPin* ExecVariablesIn = Schema->FindExecutionPin(*AssignmentNode, EGPD_Input);
	ExecChain->MakeLinkTo(ExecVariablesIn);
	ExecChain = Schema->FindExecutionPin(*AssignmentNode, EGPD_Output);

	// Run thru each property
	TSet<FName> PropertiesBeingSet;

	for (auto TargetPinIt = AssignmentNode->Pins.CreateIterator(); TargetPinIt; ++TargetPinIt)
	{
		UEdGraphPin* TargetPin = *TargetPinIt;
		FName PropertyName(TargetPin->PinName);

		// Does it get serviced by this handler?
		if (FAnimNodeSinglePropertyHandler* SourceInfo = Record.ServicedProperties.Find(PropertyName))
		{
			if (TargetPin->PinType.IsArray())
			{
				// Grab the array that we need to set members for
				UK2Node_StructMemberGet* FetchArrayNode = SpawnIntermediateNode<UK2Node_StructMemberGet>(VisualAnimNode, ConsolidatedEventGraph);
				FetchArrayNode->VariableReference.SetSelfMember(Record.NodeVariableProperty->GetFName());
				FetchArrayNode->StructType = Record.NodeVariableProperty->Struct;
				FetchArrayNode->AllocatePinsForSingleMemberGet(PropertyName);

				UEdGraphPin* ArrayVariableNode = FetchArrayNode->FindPin(PropertyName);

				if (SourceInfo->CopyRecords.Num() > 0)
				{
					// Set each element in the array
					for (FPropertyCopyRecord& CopyRecord : SourceInfo->CopyRecords)
					{
						int32 ArrayIndex = CopyRecord.DestArrayIndex;
						UEdGraphPin* DestPin = CopyRecord.DestPin;

						// Create an array element set node
						UK2Node_CallArrayFunction* ArrayNode = SpawnIntermediateNode<UK2Node_CallArrayFunction>(VisualAnimNode, ConsolidatedEventGraph);
						ArrayNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UKismetArrayLibrary, Array_Set), UKismetArrayLibrary::StaticClass());
						ArrayNode->AllocateDefaultPins();

						// Connect the execution chain
						ExecChain->MakeLinkTo(ArrayNode->GetExecPin());
						ExecChain = ArrayNode->GetThenPin();

						// Connect the input array
						UEdGraphPin* TargetArrayPin = ArrayNode->FindPinChecked(TEXT("TargetArray"));
						TargetArrayPin->MakeLinkTo(ArrayVariableNode);
						ArrayNode->PinConnectionListChanged(TargetArrayPin);

						// Set the array index
						UEdGraphPin* TargetIndexPin = ArrayNode->FindPinChecked(TEXT("Index"));
						TargetIndexPin->DefaultValue = FString::FromInt(ArrayIndex);

						// Wire up the data input
						UEdGraphPin* TargetItemPin = ArrayNode->FindPinChecked(TEXT("Item"));
						TargetItemPin->CopyPersistentDataFromOldPin(*DestPin);
						MessageLog.NotifyIntermediatePinCreation(TargetItemPin, DestPin);
						DestPin->BreakAllPinLinks();
					}
				}
			}
			else
			{
				check(!TargetPin->PinType.IsContainer())
					// Single property
					if (SourceInfo->CopyRecords.Num() > 0 && SourceInfo->CopyRecords[0].DestPin != nullptr)
					{
						UEdGraphPin* DestPin = SourceInfo->CopyRecords[0].DestPin;

						PropertiesBeingSet.Add(DestPin->PinName);
						TargetPin->CopyPersistentDataFromOldPin(*DestPin);
						MessageLog.NotifyIntermediatePinCreation(TargetPin, DestPin);
						DestPin->BreakAllPinLinks();
					}
			}
		}
	}

	// Remove any unused pins from the assignment node to avoid smashing constant values
	for (int32 PinIndex = 0; PinIndex < AssignmentNode->ShowPinForProperties.Num(); ++PinIndex)
	{
		FOptionalPinFromProperty& TestProperty = AssignmentNode->ShowPinForProperties[PinIndex];
		TestProperty.bShowPin = PropertiesBeingSet.Contains(TestProperty.PropertyName);
	}
	AssignmentNode->ReconstructNode();
}

void FPixel2DAnimBlueprintCompilerContext::CreateEvaluationHandlerInstance(UPixel2DAnimGraphNode_Base* VisualAnimNode, FEvaluationHandlerRecord& Record)
{
	// Shouldn't create a handler if there is nothing to work with
	check(Record.ServicedProperties.Num() > 0);
	check(Record.NodeVariableProperty != nullptr);
	check(Record.bServicesInstanceProperties);

	// Use the node GUID for a stable name across compiles
	FString FunctionName = FString::Printf(TEXT("%s_%s_%s_%s"), *Record.EvaluationHandlerProperty->GetName(), *VisualAnimNode->GetOuter()->GetName(), *VisualAnimNode->GetClass()->GetName(), *VisualAnimNode->NodeGuid.ToString());
	Record.HandlerFunctionName = FName(*FunctionName);

	// check function name isnt already used (data exists that can contain duplicate GUIDs) and apply a numeric extension until it is unique
	int32 ExtensionIndex = 0;
	FName* ExistingName = HandlerFunctionNames.Find(Record.HandlerFunctionName);
	while (ExistingName != nullptr)
	{
		FunctionName = FString::Printf(TEXT("%s_%s_%s_%s_%d"), *Record.EvaluationHandlerProperty->GetName(), *VisualAnimNode->GetOuter()->GetName(), *VisualAnimNode->GetClass()->GetName(), *VisualAnimNode->NodeGuid.ToString(), ExtensionIndex);
		Record.HandlerFunctionName = FName(*FunctionName);
		ExistingName = HandlerFunctionNames.Find(Record.HandlerFunctionName);
		ExtensionIndex++;
	}

	HandlerFunctionNames.Add(Record.HandlerFunctionName);

	// Add a custom event in the graph
	UK2Node_CustomEvent* EntryNode = SpawnIntermediateNode<UK2Node_CustomEvent>(VisualAnimNode, ConsolidatedEventGraph);
	EntryNode->bInternalEvent = true;
	EntryNode->CustomFunctionName = Record.HandlerFunctionName;
	EntryNode->AllocateDefaultPins();

	// The ExecChain is the current exec output pin in the linear chain
	UEdGraphPin* ExecChain = Schema->FindExecutionPin(*EntryNode, EGPD_Output);

	// Need to create a variable set call for each serviced property in the handler
	for (TPair<FName, FAnimNodeSinglePropertyHandler>& PropHandlerPair : Record.ServicedProperties)
	{
		FAnimNodeSinglePropertyHandler& PropHandler = PropHandlerPair.Value;
		FName PropertyName = PropHandlerPair.Key;

		// Should be true, we only want to deal with instance targets in here
		check(PropHandler.bInstanceIsTarget);

		for (FPropertyCopyRecord& CopyRecord : PropHandler.CopyRecords)
		{
			// New set node for the property
			UK2Node_VariableSet* VarAssignNode = SpawnIntermediateNode<UK2Node_VariableSet>(VisualAnimNode, ConsolidatedEventGraph);
			VarAssignNode->VariableReference.SetSelfMember(CopyRecord.DestProperty->GetFName());
			VarAssignNode->AllocateDefaultPins();

			// Wire up the exec line, and update the end of the chain
			UEdGraphPin* ExecVariablesIn = Schema->FindExecutionPin(*VarAssignNode, EGPD_Input);
			ExecChain->MakeLinkTo(ExecVariablesIn);
			ExecChain = Schema->FindExecutionPin(*VarAssignNode, EGPD_Output);

			// Find the property pin on the set node and configure
			for (UEdGraphPin* TargetPin : VarAssignNode->Pins)
			{
				if (TargetPin->PinType.IsContainer())
				{
					// Currently unsupported
					continue;
				}

				FName PinPropertyName(TargetPin->PinName);

				if (PinPropertyName == PropertyName)
				{
					// This is us, wire up the variable
					UEdGraphPin* DestPin = CopyRecord.DestPin;

					// Copy the data (link up to the source nodes)
					TargetPin->CopyPersistentDataFromOldPin(*DestPin);
					MessageLog.NotifyIntermediatePinCreation(TargetPin, DestPin);

					// Old pin needs to not be connected now - break all its links
					DestPin->BreakAllPinLinks();

					break;
				}
			}

			//VarAssignNode->ReconstructNode();
		}
	}
}

void FPixel2DAnimBlueprintCompilerContext::ProcessAnimationNode(UPixel2DAnimGraphNode_Base* VisualAnimNode)
{
	// Early out if this node has already been processed
	if (AllocatedAnimNodes.Contains(VisualAnimNode))
	{
		return;
	}

	// Make sure the visual node has a runtime node template
	const UScriptStruct* NodeType = VisualAnimNode->GetFNodeType();
	if (NodeType == NULL)
	{
		MessageLog.Error(TEXT("@@ has no animation node member"), VisualAnimNode);
		return;
	}

	// Give the visual node a chance to do validation
	{
		const int32 PreValidationErrorCount = MessageLog.NumErrors;
		VisualAnimNode->BakeDataDuringCompilation(MessageLog);
		if (MessageLog.NumErrors != PreValidationErrorCount)
		{
			return;
		}
	}

	// Create a property for the node
	const FString NodeVariableName = ClassScopeNetNameMap.MakeValidName(VisualAnimNode);

	const UPixel2DAnimGraphSchema* AnimGraphDefaultSchema = GetDefault<UPixel2DAnimGraphSchema>();

	FEdGraphPinType NodeVariableType;
	NodeVariableType.PinCategory = UPixel2DAnimGraphSchema::PC_Struct;
	NodeVariableType.PinSubCategoryObject = MakeWeakObjectPtr(const_cast<UScriptStruct*>(NodeType));

	FStructProperty* NewProperty = CastField<FStructProperty>(CreateVariable(FName(*NodeVariableName), NodeVariableType));

	if (NewProperty == NULL)
	{
		MessageLog.Error(TEXT("Failed to create node property for @@"), VisualAnimNode);
	}

	// Register this node with the compile-time data structures
	const int32 AllocatedIndex = AllocateNodeIndexCounter++;
	AllocatedAnimNodes.Add(VisualAnimNode, NewProperty);
	AllocatedNodePropertiesToNodes.Add(NewProperty, VisualAnimNode);
	AllocatedAnimNodeIndices.Add(VisualAnimNode, AllocatedIndex);
	AllocatedPropertiesByIndex.Add(AllocatedIndex, NewProperty);

	UPixel2DAnimGraphNode_Base* TrueSourceObject = MessageLog.FindSourceObjectTypeChecked<UPixel2DAnimGraphNode_Base>(VisualAnimNode);
	SourceNodeToProcessedNodeMap.Add(TrueSourceObject, VisualAnimNode);

	// Register the slightly more permanent debug information
	NewSpriteAnimBlueprintClass->NodeGuidToIndexMap.Add(TrueSourceObject->NodeGuid, AllocatedIndex);

	// Node-specific compilation that requires compiler state info
	if (UPixel2DAnimGraphNode_StateMachineBase* StateMachineInstance = Cast<UPixel2DAnimGraphNode_StateMachineBase>(VisualAnimNode))
	{
		// Compile the state machine
		ProcessStateMachine(StateMachineInstance);
	}

	// Record pose pins for later patchup and gather pins that have an associated evaluation handler
	TMap<FName, FEvaluationHandlerRecord> StructEvalHandlers;

	for (auto SourcePinIt = VisualAnimNode->Pins.CreateIterator(); SourcePinIt; ++SourcePinIt)
	{
		UEdGraphPin* SourcePin = *SourcePinIt;
		bool bConsumed = false;

		// Register pose links for future use
		if ((SourcePin->Direction == EGPD_Input) && (AnimGraphDefaultSchema->IsPosePin(SourcePin->PinType)))
		{
			// Input pose pin, going to need to be linked up
			FPixel2DPoseLinkMappingRecord LinkRecord = VisualAnimNode->GetLinkIDLocation(NodeType, SourcePin);
			if (LinkRecord.IsValid())
			{
				ValidPoseLinkList.Add(LinkRecord);
				bConsumed = true;
			}
		}
		else
		{
			// The property source for our data, either the struct property for an anim node, or the
			// owning anim instance if using a sub instance node.
			FProperty* SourcePinProperty = nullptr;
			int32 SourceArrayIndex = INDEX_NONE;

			{
				VisualAnimNode->GetPinAssociatedProperty(NodeType, SourcePin, /*out*/ SourcePinProperty, /*out*/ SourceArrayIndex);
			}

			if (SourcePinProperty != NULL)
			{
				if (SourcePin->LinkedTo.Num() == 0)
				{
					// Literal that can be pushed into the CDO instead of re-evaluated every frame
					new (ValidAnimNodePinConstants) FEffectiveConstantRecord(NewProperty, SourcePin, SourcePinProperty, SourceArrayIndex);
					bConsumed = true;
				}
				else
				{
					// Dynamic value that needs to be wired up and evaluated each frame
					const FString& EvaluationHandlerStr = SourcePinProperty->GetMetaData(AnimGraphDefaultSchema->NAME_OnEvaluate);
					FName EvaluationHandlerName(*EvaluationHandlerStr);
					if (EvaluationHandlerName == NAME_None)
					{
						EvaluationHandlerName = AnimGraphDefaultSchema->DefaultEvaluationHandlerName;
					}

					FEvaluationHandlerRecord& EvalHandler = StructEvalHandlers.FindOrAdd(EvaluationHandlerName);

					EvalHandler.RegisterPin(SourcePin, SourcePinProperty, SourceArrayIndex);

					bConsumed = true;
				}
			}
		}

		if (!bConsumed && (SourcePin->Direction == EGPD_Input))
		{
			//REVISIT: It's probably OK to have certain pins ignored eventually, but this is very helpful during development
			MessageLog.Note(TEXT("@@ was visible but ignored"), SourcePin);
		}
	}

	// Match the associated property to each evaluation handler
	for (TFieldIterator<FProperty> NodePropIt(NodeType); NodePropIt; ++NodePropIt)
	{
		if (FStructProperty* StructProp = CastField<FStructProperty>(*NodePropIt))
		{
			if (StructProp->Struct == FPaper2DExposedValueHandler::StaticStruct())
			{
				// Register this property to the list of pins that need to be updated
				// (it's OK if there isn't an entry for this handler, it means that the values are static and don't need to be calculated every frame)
				FName EvaluationHandlerName(StructProp->GetFName());

				FEvaluationHandlerRecord* pRecord = StructEvalHandlers.Find(EvaluationHandlerName);

				if (pRecord)
				{
					pRecord->NodeVariableProperty = NewProperty;
					pRecord->EvaluationHandlerProperty = StructProp;
				}
			}
		}
	}

	// Generate a new event to update the value of these properties
	for (auto HandlerIt = StructEvalHandlers.CreateIterator(); HandlerIt; ++HandlerIt)
	{
		FName EvaluationHandlerName = HandlerIt.Key();
		FEvaluationHandlerRecord& Record = HandlerIt.Value();

		if (Record.IsValid())
		{

			// Disable fast-path generation for nativized anim BPs, we dont run the VM anyways and 
			// the property names are 'decorated' by the backend, so records dont match.
			if (Blueprint->NativizationFlag == EBlueprintNativizationFlag::Disabled)
			{
				// build fast path copy records here
				// we need to do this at this point as they rely on traversing the original wire path
				// to determine source data. After we call CreateEvaluationHandlerStruct (etc) the original 
				// graph is modified to hook up to the evaluation handler custom functions & pins are no longer
				// available
				Record.BuildFastPathCopyRecords();
			}

			if (Record.bServicesInstanceProperties)
			{
				CreateEvaluationHandlerInstance(VisualAnimNode, Record);
			}
			else
			{
				CreateEvaluationHandlerStruct(VisualAnimNode, Record);
			}

			ValidEvaluationHandlerList.Add(Record);
		}
		else
		{
			MessageLog.Error(*FString::Printf(TEXT("A property on @@ references a non-existent %s property named %s"), *(AnimGraphDefaultSchema->NAME_OnEvaluate.ToString()), *(EvaluationHandlerName.ToString())), VisualAnimNode);
		}
	}
}

int32 FPixel2DAnimBlueprintCompilerContext::GetAllocationIndexOfNode(UPixel2DAnimGraphNode_Base* VisualAnimNode)
{
	ProcessAnimationNode(VisualAnimNode);
	int32* pResult = AllocatedAnimNodeIndices.Find(VisualAnimNode);
	return (pResult != NULL) ? *pResult : INDEX_NONE;
}

void FPixel2DAnimBlueprintCompilerContext::PruneIsolatedAnimationNodes(const TArray<UPixel2DAnimGraphNode_Base*>& RootSet, TArray<UPixel2DAnimGraphNode_Base*>& GraphNodes)
{
	struct FNodeVisitorDownPoseWires
	{
		TSet<UEdGraphNode*> VisitedNodes;
		const UPixel2DAnimGraphSchema* Schema;

		FNodeVisitorDownPoseWires()
		{
			Schema = GetDefault<UPixel2DAnimGraphSchema>();
		}

		void TraverseNodes(UEdGraphNode* Node)
		{
			VisitedNodes.Add(Node);

			// Follow every exec output pin
			for (int32 i = 0; i < Node->Pins.Num(); ++i)
			{
				UEdGraphPin* MyPin = Node->Pins[i];

				if ((MyPin->Direction == EGPD_Input) && (Schema->IsPosePin(MyPin->PinType)))
				{
					for (int32 j = 0; j < MyPin->LinkedTo.Num(); ++j)
					{
						UEdGraphPin* OtherPin = MyPin->LinkedTo[j];
						UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
						if (!VisitedNodes.Contains(OtherNode))
						{
							TraverseNodes(OtherNode);
						}
					}
				}
			}
		}
	};

	// Prune the nodes that aren't reachable via an animation pose link
	FNodeVisitorDownPoseWires Visitor;

	for (auto RootIt = RootSet.CreateConstIterator(); RootIt; ++RootIt)
	{
		UPixel2DAnimGraphNode_Base* RootNode = *RootIt;
		Visitor.TraverseNodes(RootNode);
	}

	for (int32 NodeIndex = 0; NodeIndex < GraphNodes.Num(); ++NodeIndex)
	{
		UPixel2DAnimGraphNode_Base* Node = GraphNodes[NodeIndex];
		if (!Visitor.VisitedNodes.Contains(Node) && !IsNodePure(Node))
		{
			Node->BreakAllNodeLinks();
			GraphNodes.RemoveAtSwap(NodeIndex);
			--NodeIndex;
		}
	}
}

void FPixel2DAnimBlueprintCompilerContext::ProcessAnimationNodesGivenRoot(TArray<UPixel2DAnimGraphNode_Base*>& AnimNodeList, const TArray<UPixel2DAnimGraphNode_Base*>& RootSet)
{
	// Now prune based on the root set
	if (MessageLog.NumErrors == 0)
	{
		PruneIsolatedAnimationNodes(RootSet, AnimNodeList);
	}

	// Process the remaining nodes
	for (auto SourceNodeIt = AnimNodeList.CreateIterator(); SourceNodeIt; ++SourceNodeIt)
	{
		UPixel2DAnimGraphNode_Base* VisualAnimNode = *SourceNodeIt;
		ProcessAnimationNode(VisualAnimNode);
	}
}

void FPixel2DAnimBlueprintCompilerContext::ProcessAllAnimationNodes()
{
	// Validate the graph
	ValidateGraphIsWellFormed(ConsolidatedEventGraph);

	// Build the raw node list
	TArray<UPixel2DAnimGraphNode_Base*> AnimNodeList;
	ConsolidatedEventGraph->GetNodesOfClass<UPixel2DAnimGraphNode_Base>(/*out*/ AnimNodeList);

	TArray<UK2Node_TransitionRuleGetter*> Getters;
	ConsolidatedEventGraph->GetNodesOfClass<UK2Node_TransitionRuleGetter>(/*out*/ Getters);

	// Get anim getters from the root anim graph (processing the nodes below will collect them in nested graphs)
	TArray<UK2Node_FlipbookGetter*> RootGraphAnimGetters;
	ConsolidatedEventGraph->GetNodesOfClass<UK2Node_FlipbookGetter>(RootGraphAnimGetters);

	// Find the root node
	UPixel2DAnimGraphNode_Root* PrePhysicsRoot = NULL;
	TArray<UPixel2DAnimGraphNode_Base*> RootSet;

	AllocateNodeIndexCounter = 0;
	NewSpriteAnimBlueprintClass->RootAnimNodeIndex = 0;//INDEX_NONE;

	for (auto SourceNodeIt = AnimNodeList.CreateIterator(); SourceNodeIt; ++SourceNodeIt)
	{
		UPixel2DAnimGraphNode_Base* SourceNode = *SourceNodeIt;
		UPixel2DAnimGraphNode_Base* TrueNode = MessageLog.FindSourceObjectTypeChecked<UPixel2DAnimGraphNode_Base>(SourceNode);
		TrueNode->BlueprintUsage = EBlueprintUsage::NoProperties;

		if (UPixel2DAnimGraphNode_Root* PossibleRoot = Cast<UPixel2DAnimGraphNode_Root>(SourceNode))
		{
			if (UPixel2DAnimGraphNode_Root* Root = ExactCast<UPixel2DAnimGraphNode_Root>(PossibleRoot))
			{
				if (PrePhysicsRoot != NULL)
				{
					MessageLog.Error(*LOCTEXT("ExpectedOneFunctionEntry_Error", "Expected only one animation root, but found both @@ and @@").ToString(),
						PrePhysicsRoot, Root);
				}
				else
				{
					RootSet.Add(Root);
					PrePhysicsRoot = Root;
				}
			}
		}
	}

	if (PrePhysicsRoot != NULL)
	{
		// Process the animation nodes
		ProcessAnimationNodesGivenRoot(AnimNodeList, RootSet);

	//    // Process the getter nodes in the graph if there were any
	//    for (auto GetterIt = Getters.CreateIterator(); GetterIt; ++GetterIt)
	//    {
	//        ProcessTransitionGetter(*GetterIt, NULL); // transition nodes should not appear at top-level
	//    }

		// Wire root getters
		for (UK2Node_FlipbookGetter* RootGraphGetter : RootGraphAnimGetters)
		{
			AutoWireFlipbookGetter(RootGraphGetter, nullptr);
		}

		// Wire nested getters
		for (UK2Node_FlipbookGetter* Getter : FoundGetterNodes)
		{
			AutoWireFlipbookGetter(Getter, nullptr);
		}

		NewSpriteAnimBlueprintClass->RootAnimNodeIndex = GetAllocationIndexOfNode(PrePhysicsRoot);
	}
	else
	{
		//MessageLog.Error(*LOCTEXT("ExpectedAFunctionEntry_Error", "Expected an animation root, but did not find one").ToString());
	}
}

int32 FPixel2DAnimBlueprintCompilerContext::ExpandGraphAndProcessNodes(UEdGraph* SourceGraph, UPixel2DAnimGraphNode_Base* SourceRootNode, UPixel2DAnimStateTransitionNode* TransitionNode, TArray<UEdGraphNode*>* ClonedNodes)
{
	// Clone the nodes from the source graph
	UEdGraph* ClonedGraph = FEdGraphUtilities::CloneGraph(SourceGraph, NULL, &MessageLog, true);

	// Grab all the animation nodes and find the corresponding root node in the cloned set
	UPixel2DAnimGraphNode_Base* TargetRootNode = NULL;
	TArray<UPixel2DAnimGraphNode_Base*> AnimNodeList;
	TArray<UK2Node_TransitionRuleGetter*> Getters;
	TArray<UK2Node_FlipbookGetter*> AnimGetterNodes;

	for (auto NodeIt = ClonedGraph->Nodes.CreateIterator(); NodeIt; ++NodeIt)
	{
		UEdGraphNode* Node = *NodeIt;

		if (UK2Node_TransitionRuleGetter* GetterNode = Cast<UK2Node_TransitionRuleGetter>(Node))
		{
			Getters.Add(GetterNode);
		}
		else if (UK2Node_FlipbookGetter* NewGetterNode = Cast<UK2Node_FlipbookGetter>(Node))
		{
			AnimGetterNodes.Add(NewGetterNode);
		}
		else if (UPixel2DAnimGraphNode_Base* TestNode = Cast<UPixel2DAnimGraphNode_Base>(Node))
		{
			AnimNodeList.Add(TestNode);

			//@TODO: There ought to be a better way to determine this
			if (MessageLog.FindSourceObject(TestNode) == MessageLog.FindSourceObject(SourceRootNode))
			{
				TargetRootNode = TestNode;
			}
		}

		if (ClonedNodes != NULL)
		{
			ClonedNodes->Add(Node);
		}
	}
	check(TargetRootNode);

	// Move the cloned nodes into the consolidated event graph
	const bool bIsLoading = Blueprint->bIsRegeneratingOnLoad || IsAsyncLoading();
	const bool bIsCompiling = Blueprint->bBeingCompiled;
	ClonedGraph->MoveNodesToAnotherGraph(ConsolidatedEventGraph, bIsLoading, bIsCompiling);

	// Process any animation nodes
	{
		TArray<UPixel2DAnimGraphNode_Base*> RootSet;
		RootSet.Add(TargetRootNode);
		ProcessAnimationNodesGivenRoot(AnimNodeList, RootSet);
	}

	// Process the getter nodes in the graph if there were any
	for (auto GetterIt = Getters.CreateIterator(); GetterIt; ++GetterIt)
	{
		//ProcessTransitionGetter(*GetterIt, TransitionNode);
	}

	// Wire anim getter nodes
	for (UK2Node_FlipbookGetter* GetterNode : AnimGetterNodes)
	{
		FoundGetterNodes.Add(GetterNode);
	}

	// Returns the index of the processed cloned version of SourceRootNode
	return GetAllocationIndexOfNode(TargetRootNode);
}

void FPixel2DAnimBlueprintCompilerContext::ProcessStateMachine(UPixel2DAnimGraphNode_StateMachineBase* StateMachineInstance)
{
	struct FMachineCreator
	{
	public:
		int32 MachineIndex;
		TMap<UPixel2DAnimStateNodeBase*, int32> StateIndexTable;
		TMap<UPixel2DAnimStateTransitionNode*, int32> TransitionIndexTable;
		UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintClass;
		UPixel2DAnimGraphNode_StateMachineBase* StateMachineInstance;
		FCompilerResultsLog& MessageLog;
	public:
		FMachineCreator(FCompilerResultsLog& InMessageLog, UPixel2DAnimGraphNode_StateMachineBase* InStateMachineInstance, int32 InMachineIndex, UPixel2DAnimBlueprintGeneratedClass* InNewClass)
			: MachineIndex(InMachineIndex)
			, AnimBlueprintClass(InNewClass)
			, StateMachineInstance(InStateMachineInstance)
			, MessageLog(InMessageLog)
		{
			StateMachineInstance->GetNode().StateMachineIndexInClass = MachineIndex;

			FBakedAnimationStateMachine& BakedMachine = GetMachine();
			BakedMachine.MachineName = StateMachineInstance->EditorStateMachineGraph->GetFName();
			BakedMachine.InitialState = INDEX_NONE;
		}

		FBakedAnimationStateMachine& GetMachine()
		{
			return AnimBlueprintClass->BakedStateMachines[MachineIndex];
		}

		FPixel2DStateMachineDebugData& GetMachineSpecificDebugData()
		{
			UPixel2DAnimStateMachineGraph* SourceGraph = MessageLog.FindSourceObjectTypeChecked<UPixel2DAnimStateMachineGraph>(StateMachineInstance->EditorStateMachineGraph);
			return AnimBlueprintClass->GetAnimBlueprintDebugData().StateMachineDebugData.FindOrAdd(SourceGraph);
		}

		int32 FindOrAddState(UPixel2DAnimStateNodeBase* StateNode)
		{
			if (int32* pResult = StateIndexTable.Find(StateNode))
			{
				return *pResult;
			}
			else
			{
				FBakedAnimationStateMachine& BakedMachine = GetMachine();

				const int32 StateIndex = BakedMachine.States.Num();
				StateIndexTable.Add(StateNode, StateIndex);
				new (BakedMachine.States) FBakedAnimationState();

				UPixel2DAnimStateNodeBase* SourceNode = MessageLog.FindSourceObjectTypeChecked<UPixel2DAnimStateNodeBase>(StateNode);
				GetMachineSpecificDebugData().NodeToStateIndex.Add(SourceNode, StateIndex);

				return StateIndex;
			}
		}

		int32 FindOrAddTransition(UPixel2DAnimStateTransitionNode* TransitionNode)
		{
			if (int32* pResult = TransitionIndexTable.Find(TransitionNode))
			{
				return *pResult;
			}
			else
			{
				FBakedAnimationStateMachine& BakedMachine = GetMachine();

				const int32 TransitionIndex = BakedMachine.Transitions.Num();
				TransitionIndexTable.Add(TransitionNode, TransitionIndex);
				new (BakedMachine.Transitions) FAnimationTransitionBetweenStates();

				return TransitionIndex;
			}
		}

		void Validate()
		{
			FBakedAnimationStateMachine& BakedMachine = GetMachine();

			// Make sure there is a valid entry point
			if (BakedMachine.InitialState == INDEX_NONE)
			{
				MessageLog.Warning(*LOCTEXT("NoEntryNode", "There was no entry state connection in @@").ToString(), StateMachineInstance);
				BakedMachine.InitialState = 0;
			}
			else
			{
				// Make sure the entry node is a state and not a conduit
				if (BakedMachine.States[BakedMachine.InitialState].bIsAConduit)
				{
					UEdGraphNode* StateNode = NULL; // GetMachineSpecificDebugData().FindNodeFromStateIndex(BakedMachine.InitialState);
					MessageLog.Error(*LOCTEXT("BadStateEntryNode", "A conduit (@@) cannot be used as the entry node for a state machine").ToString(), StateNode);
				}
			}
		}
	};

	if (StateMachineInstance->EditorStateMachineGraph == NULL)
	{
		MessageLog.Error(*LOCTEXT("BadStateMachineNoGraph", "@@ does not have a corresponding graph").ToString(), StateMachineInstance);
		return;
	}

	TMap<UPixel2DAnimGraphNode_TransitionResult*, int32> AlreadyMergedTransitionList;

	const int32 MachineIndex = NewSpriteAnimBlueprintClass->BakedStateMachines.Num();
	new (NewSpriteAnimBlueprintClass->BakedStateMachines) FBakedAnimationStateMachine();
	FMachineCreator Oven(MessageLog, StateMachineInstance, MachineIndex, NewSpriteAnimBlueprintClass);

	// Map of states that contain a single player node (from state root node index to associated sequence player)
	TMap<int32, UObject*> SimplePlayerStatesMap;

	// Process all the states/transitions
	for (auto StateNodeIt = StateMachineInstance->EditorStateMachineGraph->Nodes.CreateIterator(); StateNodeIt; ++StateNodeIt)
	{
		UEdGraphNode* Node = *StateNodeIt;

		if (UPixel2DAnimStateEntryNode* EntryNode = Cast<UPixel2DAnimStateEntryNode>(Node))
		{
			// Handle the state graph entry
			FBakedAnimationStateMachine& BakedMachine = Oven.GetMachine();
			if (BakedMachine.InitialState != INDEX_NONE)
			{
				MessageLog.Error(*LOCTEXT("TooManyStateMachineEntryNodes", "Found an extra entry node @@").ToString(), EntryNode);
			}
			else if (UPixel2DAnimStateNodeBase* StartState = Cast<UPixel2DAnimStateNodeBase>(EntryNode->GetOutputNode()))
			{
				BakedMachine.InitialState = Oven.FindOrAddState(StartState);
			}
			else
			{
				MessageLog.Warning(*LOCTEXT("NoConnection", "Entry node @@ is not connected to state").ToString(), EntryNode);
			}
		}
		else if (UPixel2DAnimStateTransitionNode* TransitionNode = Cast<UPixel2DAnimStateTransitionNode>(Node))
		{
			TransitionNode->ValidateNodeDuringCompilation(MessageLog);

			const int32 TransitionIndex = Oven.FindOrAddTransition(TransitionNode);
			FAnimationTransitionBetweenStates& BakedTransition = Oven.GetMachine().Transitions[TransitionIndex];

			BakedTransition.CrossfadeDuration = TransitionNode->CrossfadeDuration;

			UPixel2DAnimStateNodeBase* PreviousState = TransitionNode->GetPreviousState();
			UPixel2DAnimStateNodeBase* NextState = TransitionNode->GetNextState();

			if ((PreviousState != NULL) && (NextState != NULL))
			{
				const int32 PreviousStateIndex = Oven.FindOrAddState(PreviousState);
				const int32 NextStateIndex = Oven.FindOrAddState(NextState);

				if (TransitionNode->Bidirectional)
				{
					MessageLog.Warning(*LOCTEXT("BidirectionalTransWarning", "Bidirectional transitions aren't supported yet @@").ToString(), TransitionNode);
				}

				BakedTransition.PreviousState = PreviousStateIndex;
				BakedTransition.NextState = NextStateIndex;
			}
			else
			{
				MessageLog.Warning(*LOCTEXT("BogusTransition", "@@ is incomplete, without a previous and next state").ToString(), TransitionNode);
				BakedTransition.PreviousState = INDEX_NONE;
				BakedTransition.NextState = INDEX_NONE;
			}
		}
		else if (UPixel2DAnimStateNode* StateNode = Cast<UPixel2DAnimStateNode>(Node))
		{
			StateNode->ValidateNodeDuringCompilation(MessageLog);

			const int32 StateIndex = Oven.FindOrAddState(StateNode);
			FBakedAnimationState& BakedState = Oven.GetMachine().States[StateIndex];

			if (StateNode->BoundGraph != NULL)
			{
				BakedState.StateName = StateNode->BoundGraph->GetFName();
				BakedState.bIsAConduit = false;
				BakedState.bAlwaysResetOnEntry = StateNode->bAlwaysResetOnEntry;

				// Process the inner graph of this state
				if (UPixel2DAnimGraphNode_StateResult* AnimGraphResultNode = CastChecked<UPixel2DAnimStateGraph>(StateNode->BoundGraph)->GetResultNode())
				{
					BakedState.StateRootNodeIndex = ExpandGraphAndProcessNodes(StateNode->BoundGraph, AnimGraphResultNode);
				}
				else
				{
					BakedState.StateRootNodeIndex = INDEX_NONE;
					MessageLog.Error(*LOCTEXT("StateWithNoResult", "@@ has no result node").ToString(), StateNode);
				}
			}
			else
			{
				BakedState.StateName = NAME_None;
				MessageLog.Error(*LOCTEXT("StateWithBadGraph", "@@ has no bound graph").ToString(), StateNode);
			}

			// If this check fires, then something in the machine has changed causing the states array to not
			// be a separate allocation, and a state machine inside of this one caused stuff to shift around
			checkSlow(&BakedState == &(Oven.GetMachine().States[StateIndex]));
		}
	}

	// Process transitions after all the states because getters within custom graphs may want to
	// reference back to other states, which are only valid if they have already been baked
	for (auto StateNodeIt = Oven.StateIndexTable.CreateIterator(); StateNodeIt; ++StateNodeIt)
	{
		UPixel2DAnimStateNodeBase* StateNode = StateNodeIt.Key();
		const int32 StateIndex = StateNodeIt.Value();

		FBakedAnimationState& BakedState = Oven.GetMachine().States[StateIndex];

		// Add indices to all player nodes
		TArray<UEdGraph*> GraphsToCheck;
		TArray<UPixel2DAnimGraphNode_AssetPlayerBase*> AssetPlayerNodes;
		GraphsToCheck.Add(StateNode->GetBoundGraph());
		StateNode->GetBoundGraph()->GetAllChildrenGraphs(GraphsToCheck);

		for (UEdGraph* ChildGraph : GraphsToCheck)
		{
			ChildGraph->GetNodesOfClass(AssetPlayerNodes);
		}

		for (UPixel2DAnimGraphNode_AssetPlayerBase* Node : AssetPlayerNodes)
		{
			if (int32* IndexPtr = NewSpriteAnimBlueprintClass->NodeGuidToIndexMap.Find(Node->NodeGuid))
			{
				BakedState.PlayerNodeIndices.Add(*IndexPtr);
			}
		}

		// Handle all the transitions out of this node
		TArray<class UPixel2DAnimStateTransitionNode*> TransitionList;
		StateNode->GetTransitionList(/*out*/ TransitionList, /*bWantSortedList=*/ true);

		for (auto TransitionIt = TransitionList.CreateIterator(); TransitionIt; ++TransitionIt)
		{
			UPixel2DAnimStateTransitionNode* TransitionNode = *TransitionIt;
			const int32 TransitionIndex = Oven.FindOrAddTransition(TransitionNode);

			FBakedStateExitTransition& Rule = *new (BakedState.Transitions) FBakedStateExitTransition();
			Rule.bDesiredTransitionReturnValue = (TransitionNode->GetPreviousState() == StateNode);
			Rule.TransitionIndex = TransitionIndex;

			if (UPixel2DAnimGraphNode_TransitionResult* TransitionResultNode = CastChecked<UPixel2DAnimTransitionGraph>(TransitionNode->BoundGraph)->GetResultNode())
			{
				if (int32* pIndex = AlreadyMergedTransitionList.Find(TransitionResultNode))
				{
					Rule.CanTakeDelegateIndex = *pIndex;
				}
				else
				{
					Rule.CanTakeDelegateIndex = ExpandGraphAndProcessNodes(TransitionNode->BoundGraph, TransitionResultNode, TransitionNode);
					AlreadyMergedTransitionList.Add(TransitionResultNode, Rule.CanTakeDelegateIndex);
				}
			}
			else
			{
				Rule.CanTakeDelegateIndex = INDEX_NONE;
				MessageLog.Error(*LOCTEXT("TransitionWithNoResult", "@@ has no result node").ToString(), TransitionNode);
			}

			// Handle automatic time remaining rules
			Rule.bAutomaticRemainingTimeRule = TransitionNode->bAutomaticRuleBasedOnSequencePlayerInState;

			// Handle custom transition graphs
			Rule.CustomResultNodeIndex = INDEX_NONE;
		}
	}

	Oven.Validate();
}

void FPixel2DAnimBlueprintCompilerContext::CopyTermDefaultsToDefaultObject(UObject* DefaultObject)
{
	Super::CopyTermDefaultsToDefaultObject(DefaultObject);

	int32 LinkIndexCount = 0;
	TMap<UPixel2DAnimGraphNode_Base*, int32> LinkIndexMap;
	TMap<UPixel2DAnimGraphNode_Base*, uint8*> NodeBaseAddresses;

	// Initialize animation nodes from their templates
	for (TFieldIterator<FProperty> It(DefaultObject->GetClass(), EFieldIteratorFlags::ExcludeSuper); It; ++It)
	{
		FProperty* TargetProperty = *It;

		if (UPixel2DAnimGraphNode_Base* VisualAnimNode = AllocatedNodePropertiesToNodes.FindRef(TargetProperty))
		{
			const FStructProperty* SourceNodeProperty = VisualAnimNode->GetFNodeProperty();
			check(SourceNodeProperty != NULL);
			check(CastFieldChecked<FStructProperty>(TargetProperty)->Struct == SourceNodeProperty->Struct);

			uint8* DestinationPtr = TargetProperty->ContainerPtrToValuePtr<uint8>(DefaultObject);
			uint8* SourcePtr = SourceNodeProperty->ContainerPtrToValuePtr<uint8>(VisualAnimNode);
			TargetProperty->CopyCompleteValue(DestinationPtr, SourcePtr);

			LinkIndexMap.Add(VisualAnimNode, LinkIndexCount);
			NodeBaseAddresses.Add(VisualAnimNode, DestinationPtr);
			++LinkIndexCount;
		}
	}

	// And wire up node links
	for (auto PoseLinkIt = ValidPoseLinkList.CreateIterator(); PoseLinkIt; ++PoseLinkIt)
	{
		FPixel2DPoseLinkMappingRecord& Record = *PoseLinkIt;

		UPixel2DAnimGraphNode_Base* LinkingNode = Record.GetLinkingNode();
		UPixel2DAnimGraphNode_Base* LinkedNode = Record.GetLinkedNode();

		// @TODO this is quick solution for crash - if there were previous errors and some nodes were not added, they could still end here -
		// this check avoids that and since there are already errors, compilation won't be successful.
		// but I'd prefer stoping compilation earlier to avoid getting here in first place
		if (LinkIndexMap.Contains(LinkingNode) && LinkIndexMap.Contains(LinkedNode))
		{
			const int32 SourceNodeIndex = LinkIndexMap.FindChecked(LinkingNode);
			const int32 LinkedNodeIndex = LinkIndexMap.FindChecked(LinkedNode);
			uint8* DestinationPtr = NodeBaseAddresses.FindChecked(LinkingNode);

			Record.PatchLinkIndex(DestinationPtr, LinkedNodeIndex, SourceNodeIndex);
		}
	}

	// And patch evaluation function entry names
	for (auto EvalLinkIt = ValidEvaluationHandlerList.CreateIterator(); EvalLinkIt; ++EvalLinkIt)
	{
		FEvaluationHandlerRecord& Record = *EvalLinkIt;

		// validate fast path copy records before patching
		Record.ValidateFastPath(DefaultObject->GetClass());

		// patch either fast-path copy records or generated function names into the CDO
		Record.PatchFunctionNameAndCopyRecordsInto(DefaultObject);
	}

	// And patch in constant values that don't need to be re-evaluated every frame
	for (auto LiteralLinkIt = ValidAnimNodePinConstants.CreateIterator(); LiteralLinkIt; ++LiteralLinkIt)
	{
		FEffectiveConstantRecord& ConstantRecord = *LiteralLinkIt;

		if (!ConstantRecord.Apply(DefaultObject))
		{
			MessageLog.Error(TEXT("ICE: Failed to push literal value from @@ into CDO"), ConstantRecord.LiteralSourcePin);
		}
	}

	UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintGeneratedClass = CastChecked<UPixel2DAnimBlueprintGeneratedClass>(NewClass);

	UPixel2DAnimInstance* DefaultAnimInstance = CastChecked<UPixel2DAnimInstance>(AnimBlueprintGeneratedClass->GetDefaultObject());
//
	// copy threaded update flag to CDO
	DefaultAnimInstance->bUseMultiThreadedAnimationUpdate = AnimBlueprint->bUseMultiThreadedAnimationUpdate;

	for (const FEvaluationHandlerRecord& EvaluationHandler : ValidEvaluationHandlerList)
	{
		if (EvaluationHandler.ServicedProperties.Num() > 0)
		{
			const FAnimNodeSinglePropertyHandler& Handler = EvaluationHandler.ServicedProperties.CreateConstIterator()->Value;
			check(Handler.CopyRecords.Num() > 0);
			check(Handler.CopyRecords[0].DestPin != nullptr);
			UPixel2DAnimGraphNode_Base* Node = CastChecked<UPixel2DAnimGraphNode_Base>(Handler.CopyRecords[0].DestPin->GetOwningNode());
			UPixel2DAnimGraphNode_Base* TrueNode = MessageLog.FindSourceObjectTypeChecked<UPixel2DAnimGraphNode_Base>(Node);

			FPaper2DExposedValueHandler* HandlerPtr = EvaluationHandler.EvaluationHandlerProperty->ContainerPtrToValuePtr<FPaper2DExposedValueHandler>(EvaluationHandler.NodeVariableProperty->ContainerPtrToValuePtr<void>(DefaultAnimInstance));
			TrueNode->BlueprintUsage = HandlerPtr->BoundFunction != NAME_None ? EBlueprintUsage::UsesBlueprint : EBlueprintUsage::DoesNotUseBlueprint;
		}
	}
}

// Merges in any all ubergraph pages into the gathering ubergraph
void FPixel2DAnimBlueprintCompilerContext::MergeUbergraphPagesIn(UEdGraph* Ubergraph)
{
	Super::MergeUbergraphPagesIn(Ubergraph);

	if (bIsDerivedAnimBlueprint)
	{
		// Skip any work related to an anim graph, it's all done by the parent class
	}
	else
	{
		// Move all animation graph nodes and associated pure logic chains into the consolidated event graph
		for (int32 i = 0; i < Blueprint->FunctionGraphs.Num(); ++i)
		{
			UEdGraph* SourceGraph = Blueprint->FunctionGraphs[i];

			if (SourceGraph->Schema->IsChildOf(UPixel2DAnimGraphSchema::StaticClass()))
			{
				// Merge all the animation nodes, contents, etc... into the ubergraph
				UEdGraph* ClonedGraph = FEdGraphUtilities::CloneGraph(SourceGraph, NULL, &MessageLog, true);
				const bool bIsLoading = Blueprint->bIsRegeneratingOnLoad || IsAsyncLoading();
				const bool bIsCompiling = Blueprint->bBeingCompiled;
				ClonedGraph->MoveNodesToAnotherGraph(ConsolidatedEventGraph, bIsLoading, bIsCompiling);
			}
		}

		// Make sure we expand any split pins here before we process animation nodes.
		for (TArray<UEdGraphNode*>::TIterator NodeIt(ConsolidatedEventGraph->Nodes); NodeIt; ++NodeIt)
		{
			UK2Node* K2Node = Cast<UK2Node>(*NodeIt);
			if (K2Node != nullptr)
			{
				// We iterate the array in reverse so we can recombine split-pins (which modifies the pins array)
				for (int32 PinIndex = K2Node->Pins.Num() - 1; PinIndex >= 0; --PinIndex)
				{
					UEdGraphPin* Pin = K2Node->Pins[PinIndex];
					if (Pin->SubPins.Num() == 0)
					{
						continue;
					}

					K2Node->ExpandSplitPin(this, ConsolidatedEventGraph, Pin);
				}
			}
		}

		// Compile the animation graph
		ProcessAllAnimationNodes();
	}
}
//
void FPixel2DAnimBlueprintCompilerContext::ProcessOneFunctionGraph(UEdGraph* SourceGraph, bool bInternalFunction)
{
	if (SourceGraph->Schema->IsChildOf(UPixel2DAnimGraphSchema::StaticClass()))
	{
		// Animation graph
		// Do nothing, as this graph has already been processed
	}
	else if (SourceGraph->Schema->IsChildOf(UPixel2DAnimStateMachineSchema::StaticClass()))
	{
		// Animation state machine
		// Do nothing, as this graph has already been processed

		//@TODO: These should all have been moved to be child graphs by now
		//ensure(false);
	}
	else
	{
		// Let the regular K2 compiler handle this one
		Super::ProcessOneFunctionGraph(SourceGraph, bInternalFunction);
	}
}

void FPixel2DAnimBlueprintCompilerContext::EnsureProperGeneratedClass(UClass*& TargetUClass)
{
	if (TargetUClass && !((UObject*)TargetUClass)->IsA(UPixel2DAnimBlueprintGeneratedClass::StaticClass()))
	{
		FKismetCompilerUtilities::ConsignToOblivion(TargetUClass, Blueprint->bIsRegeneratingOnLoad);
		TargetUClass = NULL;
	}
}

void FPixel2DAnimBlueprintCompilerContext::SpawnNewClass(const FString& NewClassName)
{
	NewSpriteAnimBlueprintClass = FindObject<UPixel2DAnimBlueprintGeneratedClass>(Blueprint->GetOutermost(), *NewClassName);

	if (NewSpriteAnimBlueprintClass == NULL)
	{
		NewSpriteAnimBlueprintClass = NewObject<UPixel2DAnimBlueprintGeneratedClass>(Blueprint->GetOutermost(), FName(*NewClassName), RF_Public | RF_Transactional);
	}
	else
	{
		// Already existed, but wasn't linked in the Blueprint yet due to load ordering issues
		FBlueprintCompileReinstancer::Create(NewSpriteAnimBlueprintClass);
	}
	NewClass = NewSpriteAnimBlueprintClass;
}

void FPixel2DAnimBlueprintCompilerContext::OnNewClassSet(UBlueprintGeneratedClass* ClassToUse)
{
	NewSpriteAnimBlueprintClass = CastChecked<UPixel2DAnimBlueprintGeneratedClass>(ClassToUse);
}

void FPixel2DAnimBlueprintCompilerContext::CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOldCDO)
{
	Super::CleanAndSanitizeClass(ClassToClean, InOldCDO);

	// Make sure our typed pointer is set
	check(ClassToClean == NewClass && NewSpriteAnimBlueprintClass == NewClass);

	// Reset the baked data
	//REVISIT: Move this into PurgeClass
//    NewAnimBlueprintClass->BakedStateMachines.Empty();
//    NewAnimBlueprintClass->AnimNotifies.Empty();

	NewSpriteAnimBlueprintClass->RootAnimNodeIndex = INDEX_NONE;
	NewSpriteAnimBlueprintClass->RootAnimNodeProperty = NULL;
	NewSpriteAnimBlueprintClass->AnimNodeProperties.Empty();
}

void FPixel2DAnimBlueprintCompilerContext::FinishCompilingClass(UClass* Class)
{
	Super::FinishCompilingClass(Class);
}

void FPixel2DAnimBlueprintCompilerContext::PostCompile()
{
	Super::PostCompile();

	UPixel2DAnimBlueprintGeneratedClass* AnimBlueprintGeneratedClass = CastChecked<UPixel2DAnimBlueprintGeneratedClass>(NewClass);

	UPixel2DAnimInstance* DefaultAnimInstance = CastChecked<UPixel2DAnimInstance>(AnimBlueprintGeneratedClass->GetDefaultObject());

	// iterate all anim node and call PostCompile
	for (FStructProperty* Property : TFieldRange<FStructProperty>(AnimBlueprintGeneratedClass, EFieldIteratorFlags::IncludeSuper))
	{
		if (Property->Struct->IsChildOf(FPixel2DAnimNode_Base::StaticStruct()))
		{
			FPixel2DAnimNode_Base* AnimNode = Property->ContainerPtrToValuePtr<FPixel2DAnimNode_Base>(DefaultAnimInstance);
			AnimNode->PostCompile();
		}
	}
}

void FPixel2DAnimBlueprintCompilerContext::CreateFunctionList()
{
	// (These will now be processed after uber graph merge)

	// Build the list of functions and do preprocessing on all of them
	Super::CreateFunctionList();
}

void FPixel2DAnimBlueprintCompilerContext::PostCompileDiagnostics()
{
	FKismetCompilerContext::PostCompileDiagnostics();

	if (!bIsDerivedAnimBlueprint)
	{
		// Run thru all nodes and make sure they like the final results
		for (auto NodeIt = AllocatedAnimNodeIndices.CreateConstIterator(); NodeIt; ++NodeIt)
		{
			if (UPixel2DAnimGraphNode_Base* Node = NodeIt.Key())
			{
				Node->ValidateAnimNodePostCompile(MessageLog, NewSpriteAnimBlueprintClass, NodeIt.Value());
			}
		}
	}
}

void FPixel2DAnimBlueprintCompilerContext::AutoWireFlipbookGetter(class UK2Node_FlipbookGetter* Getter, UPixel2DAnimStateTransitionNode* InTransitionNode)
{
	UEdGraphPin* ReferencedNodeTimePin = nullptr;
	int32 ReferencedNodeIndex = INDEX_NONE;
	int32 SubNodeIndex = INDEX_NONE;

	UPixel2DAnimGraphNode_Base* ProcessedNodeCheck = NULL;

	if (UPixel2DAnimGraphNode_Base* SourceNode = Getter->SourceNode)
	{
		UPixel2DAnimGraphNode_Base* ActualSourceNode = MessageLog.FindSourceObjectTypeChecked<UPixel2DAnimGraphNode_Base>(SourceNode);

		if (UPixel2DAnimGraphNode_Base* ProcessedSourceNode = SourceNodeToProcessedNodeMap.FindRef(ActualSourceNode))
		{
			ProcessedNodeCheck = ProcessedSourceNode;

			ReferencedNodeIndex = GetAllocationIndexOfNode(ProcessedSourceNode);
		}
	}

	if (Getter->SourceStateNode)
	{
		UObject* SourceObject = MessageLog.FindSourceObject(Getter->SourceStateNode);
		if (UPixel2DAnimStateNode* SourceStateNode = Cast<UPixel2DAnimStateNode>(SourceObject))
		{
			if (FPixel2DStateMachineDebugData* DebugData = NewSpriteAnimBlueprintClass->GetAnimBlueprintDebugData().StateMachineDebugData.Find(SourceStateNode->GetGraph()))
			{
				if (int32* StateIndexPtr = DebugData->NodeToStateIndex.Find(SourceStateNode))
				{
					SubNodeIndex = *StateIndexPtr;
				}
			}
		}
	}

	check(Getter->IsNodePure());

	for (UEdGraphPin* Pin : Getter->Pins)
	{
		// Hook up autowired parameters / pins
		if (Pin->PinName == TEXT("CurrentTime"))
		{
			Pin->MakeLinkTo(ReferencedNodeTimePin);
		}
		else if (Pin->PinName == TEXT("AssetPlayerIndex") || Pin->PinName == TEXT("MachineIndex"))
		{
			Pin->DefaultValue = FString::FromInt(ReferencedNodeIndex);
		}
		else if (Pin->PinName == TEXT("StateIndex") || Pin->PinName == TEXT("TransitionIndex"))
		{
			Pin->DefaultValue = FString::FromInt(SubNodeIndex);
		}
	}
}

void FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::PatchFunctionNameAndCopyRecordsInto(UObject* TargetObject) const
{
	FPaper2DExposedValueHandler* HandlerPtr = EvaluationHandlerProperty->ContainerPtrToValuePtr<FPaper2DExposedValueHandler>(NodeVariableProperty->ContainerPtrToValuePtr<void>(TargetObject));
	HandlerPtr->CopyRecords.Empty();

	if (IsFastPath())
	{
		for (const TPair<FName, FAnimNodeSinglePropertyHandler>& ServicedPropPair : ServicedProperties)
		{
			const FName& PropertyName = ServicedPropPair.Key;
			const FAnimNodeSinglePropertyHandler& PropertyHandler = ServicedPropPair.Value;

			for (const FPropertyCopyRecord& PropertyCopyRecord : PropertyHandler.CopyRecords)
			{
				// get the correct property sizes for the type we are dealing with (array etc.)
				int32 DestPropertySize = PropertyCopyRecord.DestProperty->GetSize();
				if (FArrayProperty* DestArrayProperty = CastField<FArrayProperty>(PropertyCopyRecord.DestProperty))
				{
					DestPropertySize = DestArrayProperty->Inner->GetSize();
				}

				FPixel2DExposedValueCopyRecord CopyRecord;
				CopyRecord.DestProperty = PropertyCopyRecord.DestProperty;
				CopyRecord.DestArrayIndex = PropertyCopyRecord.DestArrayIndex == INDEX_NONE ? 0 : PropertyCopyRecord.DestArrayIndex;
				CopyRecord.SourcePropertyName = PropertyCopyRecord.SourcePropertyName;
				CopyRecord.SourceSubPropertyName = PropertyCopyRecord.SourceSubStructPropertyName;
				CopyRecord.SourceArrayIndex = 0;
				CopyRecord.Size = DestPropertySize;
				CopyRecord.PostCopyOperation = PropertyCopyRecord.Operation;
				CopyRecord.bInstanceIsTarget = PropertyHandler.bInstanceIsTarget;
				HandlerPtr->CopyRecords.Add(CopyRecord);
			}
		}
	}
	else
	{
		// not all of our pins use copy records so we will need to call our exposed value handler
		HandlerPtr->BoundFunction = HandlerFunctionName;
	}
}

static UEdGraphPin* FindFirstInputPin(UEdGraphNode* InNode)
{
	const UPixel2DAnimGraphSchema* Schema = GetDefault<UPixel2DAnimGraphSchema>();

	for (UEdGraphPin* Pin : InNode->Pins)
	{
		if (Pin && Pin->Direction == EGPD_Input && !Schema->IsExecPin(*Pin) && !Schema->IsSelfPin(*Pin))
		{
			return Pin;
		}
	}

	return nullptr;
}

static UEdGraphNode* FollowKnots(UEdGraphPin* FromPin, UEdGraphPin*& ToPin)
{
	if (FromPin->LinkedTo.Num() == 0)
	{
		return nullptr;
	}

	UEdGraphPin* LinkedPin = FromPin->LinkedTo[0];
	ToPin = LinkedPin;
	if (LinkedPin)
	{
		UEdGraphNode* LinkedNode = LinkedPin->GetOwningNode();
		UK2Node_Knot* KnotNode = Cast<UK2Node_Knot>(LinkedNode);
		while (KnotNode)
		{
			if (UEdGraphPin* InputPin = FindFirstInputPin(KnotNode))
			{
				if (InputPin->LinkedTo.Num() > 0 && InputPin->LinkedTo[0])
				{
					ToPin = InputPin->LinkedTo[0];
					LinkedNode = InputPin->LinkedTo[0]->GetOwningNode();
					KnotNode = Cast<UK2Node_Knot>(LinkedNode);
				}
				else
				{
					KnotNode = nullptr;
				}
			}
		}
		return LinkedNode;
	}

	return nullptr;
}

void FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::RegisterPin(UEdGraphPin* DestPin, FProperty* AssociatedProperty, int32 AssociatedPropertyArrayIndex)
{
	FAnimNodeSinglePropertyHandler& Handler = ServicedProperties.FindOrAdd(AssociatedProperty->GetFName());
	Handler.CopyRecords.Emplace(DestPin, AssociatedProperty, AssociatedPropertyArrayIndex);
}

void FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::BuildFastPathCopyRecords()
{
	if (GetDefault<UEngine>()->bOptimizeAnimBlueprintMemberVariableAccess)
	{
		for (TPair<FName, FAnimNodeSinglePropertyHandler>& ServicedPropPair : ServicedProperties)
		{
			for (FPropertyCopyRecord& CopyRecord : ServicedPropPair.Value.CopyRecords)
			{
				typedef bool (FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::*GraphCheckerFunc)(FPropertyCopyRecord&, UEdGraphPin*);

				GraphCheckerFunc GraphCheckerFuncs[] =
				{
					&FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::CheckForVariableGet,
					&FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::CheckForLogicalNot,
					&FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::CheckForStructMemberAccess,
				};

				for (GraphCheckerFunc& CheckFunc : GraphCheckerFuncs)
				{
					if ((this->*CheckFunc)(CopyRecord, CopyRecord.DestPin))
					{
						break;
					}
				}

				CheckForMemberOnlyAccess(CopyRecord, CopyRecord.DestPin);
			}
		}
	}
}

static FName RecoverSplitStructPinName(UEdGraphPin* OutputPin)
{
	check(OutputPin->ParentPin);

	FString PinName = OutputPin->PinName.ToString();
	const FString ParentPinName = OutputPin->ParentPin->PinName.ToString() + TEXT("_");

	PinName.ReplaceInline(*ParentPinName, TEXT(""));

	return *PinName;
}

bool FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::CheckForVariableGet(FPropertyCopyRecord& CopyRecord, UEdGraphPin* DestPin)
{
	if (DestPin)
	{
		UEdGraphPin* SourcePin = nullptr;
		if (UK2Node_VariableGet* VariableGetNode = Cast<UK2Node_VariableGet>(FollowKnots(DestPin, SourcePin)))
		{
			if (VariableGetNode && VariableGetNode->IsNodePure() && VariableGetNode->VariableReference.IsSelfContext())
			{
				if (SourcePin)
				{
					// variable get could be a 'split' struct
					if (SourcePin->ParentPin != nullptr)
					{
						CopyRecord.SourcePropertyName = VariableGetNode->VariableReference.GetMemberName();
						CopyRecord.SourceSubStructPropertyName = RecoverSplitStructPinName(SourcePin);
					}
					else
					{
						CopyRecord.SourcePropertyName = VariableGetNode->VariableReference.GetMemberName();
					}
					return true;
				}
			}
		}
	}

	return false;
}

bool FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::CheckForLogicalNot(FPropertyCopyRecord& CopyRecord, UEdGraphPin* DestPin)
{
	if (DestPin)
	{
		UEdGraphPin* SourcePin = nullptr;
		UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(FollowKnots(DestPin, SourcePin));
		if (CallFunctionNode && CallFunctionNode->FunctionReference.GetMemberName() == FName(TEXT("Not_PreBool")))
		{
			// find and follow input pin
			if (UEdGraphPin* InputPin = FindFirstInputPin(CallFunctionNode))
			{
				check(InputPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Boolean);
				if (CheckForVariableGet(CopyRecord, InputPin) || CheckForStructMemberAccess(CopyRecord, InputPin))
				{
					check(CopyRecord.SourcePropertyName != NAME_None);	// this should have been filled in by CheckForVariableGet() or CheckForStructMemberAccess() above
					CopyRecord.Operation = EPostCopyOperation::LogicalNegateBool;
					return true;
				}
			}
		}
	}

	return false;
}

/** The functions that we can safely native-break */
static const FName NativeBreakFunctionNameWhitelist[] =
{
	FName(TEXT("BreakVector")),
	FName(TEXT("BreakVector2D")),
	FName(TEXT("BreakRotator")),
};

/** Check whether a native break function can be safely used in the fast-path copy system (ie. source and dest data will be the same) */
static bool IsWhitelistedNativeBreak(const FName& InFunctionName)
{
	for (const FName& FunctionName : NativeBreakFunctionNameWhitelist)
	{
		if (InFunctionName == FunctionName)
		{
			return true;
		}
	}

	return false;
}

bool FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::CheckForStructMemberAccess(FPropertyCopyRecord& CopyRecord, UEdGraphPin* DestPin)
{
	if (DestPin)
	{
		UEdGraphPin* SourcePin = nullptr;
		if (UK2Node_BreakStruct* BreakStructNode = Cast<UK2Node_BreakStruct>(FollowKnots(DestPin, SourcePin)))
		{
			if (UEdGraphPin* InputPin = FindFirstInputPin(BreakStructNode))
			{
				if (CheckForVariableGet(CopyRecord, InputPin))
				{
					check(CopyRecord.SourcePropertyName != NAME_None);	// this should have been filled in by CheckForVariableGet() above
					CopyRecord.SourceSubStructPropertyName = SourcePin->PinName;
					return true;
				}
			}
		}
		// could be a native break
		else if (UK2Node_CallFunction* NativeBreakNode = Cast<UK2Node_CallFunction>(FollowKnots(DestPin, SourcePin)))
		{
			UFunction* Function = NativeBreakNode->FunctionReference.ResolveMember<UFunction>(UKismetMathLibrary::StaticClass());
			if (Function && Function->HasMetaData(TEXT("NativeBreakFunc")) && IsWhitelistedNativeBreak(Function->GetFName()))
			{
				if (UEdGraphPin* InputPin = FindFirstInputPin(NativeBreakNode))
				{
					if (CheckForVariableGet(CopyRecord, InputPin))
					{
						check(CopyRecord.SourcePropertyName != NAME_None);	// this should have been filled in by CheckForVariableGet() above
						CopyRecord.SourceSubStructPropertyName = SourcePin->PinName;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::CheckForMemberOnlyAccess(FPropertyCopyRecord& CopyRecord, UEdGraphPin* DestPin)
{
	const UPixel2DAnimGraphSchema* AnimGraphDefaultSchema = GetDefault<UPixel2DAnimGraphSchema>();

	if (DestPin)
	{
		// traverse pins to leaf nodes and check for member access/pure only
		TArray<UEdGraphPin*> PinStack;
		PinStack.Add(DestPin);
		while (PinStack.Num() > 0)
		{
			UEdGraphPin* CurrentPin = PinStack.Pop(false);
			for (auto& LinkedPin : CurrentPin->LinkedTo)
			{
				UEdGraphNode* LinkedNode = LinkedPin->GetOwningNode();
				if (LinkedNode)
				{
					bool bLeafNode = true;
					for (auto& Pin : LinkedNode->Pins)
					{
						if (Pin != LinkedPin && Pin->Direction == EGPD_Input && !AnimGraphDefaultSchema->IsPosePin(Pin->PinType))
						{
							bLeafNode = false;
							PinStack.Add(Pin);
						}
					}

					if (bLeafNode)
					{
						if (UK2Node_VariableGet* LinkedVariableGetNode = Cast<UK2Node_VariableGet>(LinkedNode))
						{
							if (!LinkedVariableGetNode->IsNodePure() || !LinkedVariableGetNode->VariableReference.IsSelfContext())
							{
								// only local variable access is allowed for leaf nodes 
								CopyRecord.InvalidateFastPath();
							}
						}
						else if (UK2Node_CallFunction* CallFunctionNode = Cast<UK2Node_CallFunction>(LinkedNode))
						{
							if (!CallFunctionNode->IsNodePure())
							{
								// only allow pure function calls
								CopyRecord.InvalidateFastPath();
							}
						}
						else if (!LinkedNode->IsA<UK2Node_TransitionRuleGetter>())
						{
							CopyRecord.InvalidateFastPath();
						}
					}
				}
			}
		}
	}

	return CopyRecord.IsFastPath();
}

void FPixel2DAnimBlueprintCompilerContext::FEvaluationHandlerRecord::ValidateFastPath(UClass* InCompiledClass)
{
	for (TPair<FName, FAnimNodeSinglePropertyHandler>& ServicedPropPair : ServicedProperties)
	{
		for (FPropertyCopyRecord& CopyRecord : ServicedPropPair.Value.CopyRecords)
		{
			CopyRecord.ValidateFastPath(InCompiledClass);
		}
	}
}

void FPixel2DAnimBlueprintCompilerContext::FPropertyCopyRecord::ValidateFastPath(UClass* InCompiledClass)
{
	// REVISIT
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE