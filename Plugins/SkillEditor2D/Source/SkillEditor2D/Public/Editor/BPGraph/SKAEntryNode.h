// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "SGraphNode.h"
#include "SKAEntryNode.generated.h"

/**
 * 
 */
UCLASS()
class SKILLEDITOR2D_API USKAEntryNode : public UEdGraphNode
{
	GENERATED_BODY()
	//~ Begin UEdGraphNode Interface
	USKAEntryNode();
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	//~ End UEdGraphNode Interface
	
	UEdGraphNode* GetOutputNode() const;
};


class SKILLEDITOR2D_API SGraphSKAEntryNode: public  SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphSKAEntryNode){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, USKAEntryNode* InNode);
	

};
