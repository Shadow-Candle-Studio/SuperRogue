// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimStateTransitionNode.h"
#include "SGraphNode.h"

/**
 * 
 */

class SKILLEDITOR2D_API TransitionSGraphNode: public  SGraphNode
{
public:
	SLATE_BEGIN_ARGS(TransitionSGraphNode){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UAnimStateTransitionNode* InNode);
	
	
};
