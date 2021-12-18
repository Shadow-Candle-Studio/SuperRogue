// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimStateTransitionNode.h"
#include "Object.h"
#include "SKANode.generated.h"

/**
 * state node
 */
UCLASS(MinimalAPI)
class  USKANode : public UEdGraphNode
{
	GENERATED_BODY()
public:
	 virtual void AllocateDefaultPins() override;
	 virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	 virtual UEdGraphPin* GetInputPin() const {return Pins[0];}
	 virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }
	 virtual FString GetStateName() const { return TEXT("MyState"); }
	TArray<UAnimStateTransitionNode*>& GetTransitionNodes();//The TransitionNodes to next state

private:
	TArray<UAnimStateTransitionNode*> TransitionNodes;
	
};
