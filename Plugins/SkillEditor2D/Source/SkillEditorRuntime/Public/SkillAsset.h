// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "SkillAsset.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=SkillEditor,Category="SkillEditor", HideCategories=(Object),BlueprintType,Blueprintable)
class SKILLEDITORRUNTIME_API USkillAsset : public UObject
{
	GENERATED_BODY()
public :
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Skill Editor Test")
	FText text;
	
};
