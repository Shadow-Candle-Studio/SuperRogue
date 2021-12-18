// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "SkillAsset.generated.h"

/**
 * This class Stores everything including:
 * Params of the skill
 * Sequence of the skill process
 * Events in this process:
 * Those events include Control scripts of Camera, VFX, bgm switching etc....
 */
UCLASS(ClassGroup=SkillEditor,Category="SkillEditor", HideCategories=(Object),BlueprintType,Blueprintable)
class SKILLEDITORRUNTIME_API USkillAsset : public UObject
{
	GENERATED_BODY()
public :
	//test text for the property
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category="Skill Editor Test")
	FText text;
	UPROPERTY()
	UEdGraph* AssetGraph;
};
