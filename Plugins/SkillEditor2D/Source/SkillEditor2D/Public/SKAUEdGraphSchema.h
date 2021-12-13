// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "SKAUEdGraphSchema.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class  USKAUEdGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()
public:
	USKAUEdGraphSchema(const FObjectInitializer& ObjectInitializer);
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	
};
