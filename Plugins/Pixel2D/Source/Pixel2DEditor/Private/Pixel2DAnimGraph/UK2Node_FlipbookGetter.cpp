// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UK2Node_FlipbookGetter.h"
#include "Pixel2DAnimBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Pixel2DAnimInstance.h"
#include "Pixel2DAnimGraphSchema.h"
#include "Pixel2DAnimTransitionSchema.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Pixel2DAnimGraphNode_AssetPlayerBase.h"
#include "Pixel2DAnimGraphNode_StateMachine.h"
#include "Pixel2DAnimStateNode.h"
#include "Pixel2DAnimStateMachineGraph.h"
#include "Pixel2DAnimStateTransitionNode.h"

#define LOCTEXT_NAMESPACE "UK2Node_FlipbookGetter"


void UK2Node_FlipbookGetter::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	Ar.UsingCustomVersion(FReleaseObjectVersion::GUID);

	if (Ar.CustomVer(FReleaseObjectVersion::GUID) < FReleaseObjectVersion::FixBrokenStateMachineReferencesInTransitionGetters)
	{
		RestoreStateMachineNode();
	}
}

void UK2Node_FlipbookGetter::PostPasteNode()
{
	Super::PostPasteNode();
	RestoreStateMachineNode();
}

void UK2Node_FlipbookGetter::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	TArray<UEdGraphPin*> PinsToHide;
	TArray<FString> PinNames;

	// TODO: Find a nicer way to maybe pull these down from the instance class and allow 
	// projects to add new parameters from derived instances
	PinNames.Add(TEXT("CurrentTime"));
	PinNames.Add(TEXT("AssetPlayerIndex"));
	PinNames.Add(TEXT("MachineIndex"));
	PinNames.Add(TEXT("StateIndex"));
	PinNames.Add(TEXT("TransitionIndex"));

	for (FString& PinName : PinNames)
	{
		if (UEdGraphPin* FoundPin = FindPin(PinName))
		{
			PinsToHide.Add(FoundPin);
		}
	}

	for (UEdGraphPin* Pin : PinsToHide)
	{
		Pin->bHidden = true;
	}
}

FText UK2Node_FlipbookGetter::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return CachedTitle;
}

bool UK2Node_FlipbookGetter::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Cast<UPixel2DAnimGraphSchema>(Schema) != NULL || Cast<UPixel2DAnimTransitionSchema>(Schema) != NULL;
}

void UK2Node_FlipbookGetter::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// First cache the available functions for getters
	UClass* ActionKey = GetClass();
	const UPixel2DAnimBlueprint* AnimBlueprint = Cast<UPixel2DAnimBlueprint>(ActionRegistrar.GetActionKeyFilter());
	if (AnimBlueprint && ActionRegistrar.IsOpenForRegistration(AnimBlueprint))
	{
		UClass* BPClass = *AnimBlueprint->ParentClass;
		while (BPClass && !BPClass->HasAnyClassFlags(CLASS_Native))
		{
			BPClass = BPClass->GetSuperClass();
		}

		if (BPClass)
		{
			TArray<UFunction*> AnimGetters;
			for (TFieldIterator<UFunction> FuncIter(BPClass); FuncIter; ++FuncIter)
			{
				UFunction* Func = *FuncIter;

				if (Func->HasMetaData(TEXT("AnimGetter")) && Func->HasAnyFunctionFlags(FUNC_Native))
				{
					AnimGetters.Add(Func);
				}
			}

			auto UiSpecOverride = [](const FBlueprintActionContext& /*Context*/, const IBlueprintNodeBinder::FBindingSet& Bindings, FBlueprintActionUiSpec* UiSpecOut, FText Title)
			{
				UiSpecOut->MenuName = Title;
			};

			TArray<UPixel2DAnimGraphNode_AssetPlayerBase*> AssetPlayerNodes;
			TArray<UPixel2DAnimGraphNode_StateMachine*> MachineNodes;
			TArray<UPixel2DAnimStateNode*> StateNodes;
			TArray<UPixel2DAnimStateTransitionNode*> TransitionNodes;

			FBlueprintEditorUtils::GetAllNodesOfClass(AnimBlueprint, AssetPlayerNodes);
			FBlueprintEditorUtils::GetAllNodesOfClass(AnimBlueprint, MachineNodes);
			FBlueprintEditorUtils::GetAllNodesOfClass(AnimBlueprint, StateNodes);
			FBlueprintEditorUtils::GetAllNodesOfClass(AnimBlueprint, TransitionNodes);

			for (UFunction* Getter : AnimGetters)
			{
				FPixel2DNodeSpawnData Params;
				Params.AnimInstanceClass = BPClass;
				Params.Getter = Getter;
				Params.SourceBlueprint = AnimBlueprint;
				Params.GetterContextString = Getter->GetMetaData(TEXT("GetterContext"));

				if (GetterRequiresParameter(Getter, TEXT("AssetPlayerIndex")))
				{
					// REVISIT - might support this in the future
				}
				else if (GetterRequiresParameter(Getter, TEXT("MachineIndex")))
				{
					if (GetterRequiresParameter(Getter, TEXT("StateIndex")))
					{
						for (UPixel2DAnimStateNode* StateNode : StateNodes)
						{
							// Get the state machine node from the outer chain
							UPixel2DAnimStateMachineGraph* Graph = Cast<UPixel2DAnimStateMachineGraph>(StateNode->GetOuter());
							if (Graph)
							{
								if (UPixel2DAnimGraphNode_StateMachine* MachineNode = Cast<UPixel2DAnimGraphNode_StateMachine>(Graph->GetOuter()))
								{
									Params.SourceNode = MachineNode;
								}
							}

							FText Title = FText::Format(LOCTEXT("NodeTitle", "{0} ({1})"), Getter->GetDisplayNameText(), StateNode->GetNodeTitle(ENodeTitleType::ListView));
							Params.SourceStateNode = StateNode;
							Params.CachedTitle = Title;

							UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(UK2Node_FlipbookGetter::StaticClass(), /*StateNode->GetGraph()*/nullptr, UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateUObject(const_cast<UK2Node_FlipbookGetter*>(this), &UK2Node_FlipbookGetter::PostSpawnNodeSetup, Params));
							Spawner->DynamicUiSignatureGetter = UBlueprintNodeSpawner::FUiSpecOverrideDelegate::CreateStatic(UiSpecOverride, Title);
							ActionRegistrar.AddBlueprintAction(AnimBlueprint, Spawner);
						}
					}
					else if (GetterRequiresParameter(Getter, TEXT("TransitionIndex")))
					{
						for (UPixel2DAnimStateTransitionNode* TransitionNode : TransitionNodes)
						{
							UPixel2DAnimStateMachineGraph* Graph = Cast<UPixel2DAnimStateMachineGraph>(TransitionNode->GetOuter());
							if (Graph)
							{
								if (UPixel2DAnimGraphNode_StateMachine* MachineNode = Cast<UPixel2DAnimGraphNode_StateMachine>(Graph->GetOuter()))
								{
									Params.SourceNode = MachineNode;
								}
							}

							FText Title = FText::Format(LOCTEXT("NodeTitle", "{0} ({1})"), Getter->GetDisplayNameText(), TransitionNode->GetNodeTitle(ENodeTitleType::ListView));
							Params.SourceStateNode = TransitionNode;
							Params.CachedTitle = Title;

							UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(UK2Node_FlipbookGetter::StaticClass(), /*TransitionNode->GetGraph()*/nullptr, UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateUObject(const_cast<UK2Node_FlipbookGetter*>(this), &UK2Node_FlipbookGetter::PostSpawnNodeSetup, Params));
							Spawner->DynamicUiSignatureGetter = UBlueprintNodeSpawner::FUiSpecOverrideDelegate::CreateStatic(UiSpecOverride, Title);
							ActionRegistrar.AddBlueprintAction(AnimBlueprint, Spawner);
						}
					}
					else
					{
						// Only requires the state machine
						for (UPixel2DAnimGraphNode_StateMachine* MachineNode : MachineNodes)
						{
							FText Title = FText::Format(LOCTEXT("NodeTitle", "{0} ({1})"), Getter->GetDisplayNameText(), MachineNode->GetNodeTitle(ENodeTitleType::ListView));
							Params.SourceNode = MachineNode;
							Params.CachedTitle = Title;

							UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(UK2Node_FlipbookGetter::StaticClass(), /*MachineNode*/nullptr, UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateUObject(const_cast<UK2Node_FlipbookGetter*>(this), &UK2Node_FlipbookGetter::PostSpawnNodeSetup, Params));
							Spawner->DynamicUiSignatureGetter = UBlueprintNodeSpawner::FUiSpecOverrideDelegate::CreateStatic(UiSpecOverride, Title);
							ActionRegistrar.AddBlueprintAction(AnimBlueprint, Spawner);
						}
					}
				}
				else
				{
					// Doesn't operate on a node, only need one entry
					FText Title = FText::Format(LOCTEXT("NodeTitleNoNode", "{0}"), Getter->GetDisplayNameText());
					Params.CachedTitle = Title;

					UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(UK2Node_FlipbookGetter::StaticClass(), nullptr, UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateUObject(const_cast<UK2Node_FlipbookGetter*>(this), &UK2Node_FlipbookGetter::PostSpawnNodeSetup, Params));
					Spawner->DynamicUiSignatureGetter = UBlueprintNodeSpawner::FUiSpecOverrideDelegate::CreateStatic(UiSpecOverride, Title);
					ActionRegistrar.AddBlueprintAction(AnimBlueprint, Spawner);
				}
			}
		}
	}
}

bool UK2Node_FlipbookGetter::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
	if (Filter.Context.Graphs.Num() > 0)
	{
		UEdGraph* CurrGraph = Filter.Context.Graphs[0];

		if (CurrGraph && Filter.Context.Blueprints.Num() > 0)
		{
			UPixel2DAnimBlueprint* AnimBlueprint = Cast<UPixel2DAnimBlueprint>(Filter.Context.Blueprints[0]);
			check(AnimBlueprint);

			if (SourceAnimBlueprint == AnimBlueprint)
			{
				// Get the native anim instance derived class
				UClass* NativeInstanceClass = AnimBlueprint->ParentClass;
				while (NativeInstanceClass && !NativeInstanceClass->HasAnyClassFlags(CLASS_Native))
				{
					NativeInstanceClass = NativeInstanceClass->GetSuperClass();
				}

				if (GetterClass != NativeInstanceClass)
				{
					// If the anim instance containing the getter is not the class we're currently using then bail
					return true;
				}

				const UEdGraphSchema* Schema = CurrGraph->GetSchema();

				// Bail if we aren't meant for this graph at all
				if (!IsContextValidForSchema(Schema))
				{
					return true;
				}

				if (Cast<UPixel2DAnimTransitionSchema>(Schema))
				{
					if (!SourceNode && !SourceStateNode)
					{
						// No dependancies, always allow
						return false;
					}

					// Inside a transition graph
					if (SourceNode)
					{
						if (UPixel2DAnimStateTransitionNode* TransitionNode = Cast<UPixel2DAnimStateTransitionNode>(CurrGraph->GetOuter()))
						{
							if (SourceStateNode)
							{
								if (UPixel2DAnimStateTransitionNode* SourceTransitionNode = Cast<UPixel2DAnimStateTransitionNode>(SourceStateNode))
								{
									// if we have a transition node, make sure it's the same as the one we're in
									if (SourceTransitionNode == TransitionNode)
									{
										return false;
									}
								}
								else if (UPixel2DAnimStateNode* PreviousStateNode = Cast<UPixel2DAnimStateNode>(TransitionNode->GetPreviousState()))
								{
									// Only allow actions using states that are referencing the previous state
									if (SourceStateNode == PreviousStateNode)
									{
										return false;
									}
								}
							}
							else if (UPixel2DAnimGraphNode_StateMachine* MachineNode = Cast<UPixel2DAnimGraphNode_StateMachine>(SourceNode))
							{
								// Available everywhere
								return false;
							}
							else if (UPixel2DAnimStateNode* PrevStateNode = Cast<UPixel2DAnimStateNode>(TransitionNode->GetPreviousState()))
							{
								// Make sure the attached asset node is in the source graph
								if (SourceNode && SourceNode->GetGraph() == PrevStateNode->BoundGraph)
								{
									return false;
								}
							}
						}
					}
				}
				else if (Cast<UPixel2DAnimGraphSchema>(Schema))
				{
					// Inside normal anim graph
					if (SourceStateNode)
					{
						for (UBlueprint* Blueprint : Filter.Context.Blueprints)
						{
							TArray<UPixel2DAnimStateNode*> StateNodes;
							FBlueprintEditorUtils::GetAllNodesOfClass(Blueprint, StateNodes);

							if (StateNodes.Contains(SourceStateNode))
							{
								return false;
							}
						}
					}
				}
			}
		}
	}

	return true;
}

void UK2Node_FlipbookGetter::RestoreStateMachineNode()
{
	if (SourceStateNode)
	{
		UPixel2DAnimStateMachineGraph* Graph = Cast<UPixel2DAnimStateMachineGraph>(SourceStateNode->GetOuter());
		if (Graph)
		{
			if (UPixel2DAnimGraphNode_StateMachine* MachineNode = Cast<UPixel2DAnimGraphNode_StateMachine>(Graph->GetOuter()))
			{
				SourceNode = MachineNode;
			}
		}
	}
}

bool UK2Node_FlipbookGetter::GetterRequiresParameter(const UFunction* Getter, FString ParamName) const
{
	bool bRequiresParameter = false;

	for (TFieldIterator<FProperty> PropIt(Getter); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	{
		FProperty* Prop = *PropIt;

		if (Prop->GetName() == ParamName)
		{
			bRequiresParameter = true;
			break;
		}
	}

	return bRequiresParameter;
}


void UK2Node_FlipbookGetter::PostSpawnNodeSetup(UEdGraphNode* NewNode, bool bIsTemplateNode, FPixel2DNodeSpawnData SpawnData)
{
	UK2Node_FlipbookGetter* TypedNode = CastChecked<UK2Node_FlipbookGetter>(NewNode);

	// Apply parameters
	TypedNode->SourceNode = SpawnData.SourceNode;
	TypedNode->SourceStateNode = SpawnData.SourceStateNode;
	TypedNode->GetterClass = SpawnData.AnimInstanceClass;
	TypedNode->SourceAnimBlueprint = SpawnData.SourceBlueprint;
	TypedNode->SetFromFunction((UFunction*)SpawnData.Getter);
	TypedNode->CachedTitle = SpawnData.CachedTitle;

	SpawnData.GetterContextString.ParseIntoArray(TypedNode->Contexts, TEXT("|"), 1);
}

bool UK2Node_FlipbookGetter::IsContextValidForSchema(const UEdGraphSchema* Schema) const
{
	if (Contexts.Num() == 0)
	{
		// Valid in all graphs
		return true;
	}

	for (const FString& Context : Contexts)
	{
		UClass* ClassToCheck = nullptr;

		if (Context == TEXT("Transition"))
		{
			ClassToCheck = UPixel2DAnimTransitionSchema::StaticClass();
		}

		if (Context == TEXT("AnimGraph"))
		{
			ClassToCheck = UPixel2DAnimGraphSchema::StaticClass();
		}

		return Schema->GetClass() == ClassToCheck;
	}

	return false;
}

FPixel2DNodeSpawnData::FPixel2DNodeSpawnData()
	: SourceNode(nullptr)
	, SourceStateNode(nullptr)
	, AnimInstanceClass(nullptr)
	, SourceBlueprint(nullptr)
	, Getter(nullptr)
{
}

#undef LOCTEXT_NAMESPACE
