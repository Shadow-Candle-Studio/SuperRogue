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
class SKILLEDITORRUNTIME_API USkillAsset : public UBlueprint
{
	GENERATED_BODY()
public :
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int num=22;
	//Constructor
	USkillAsset();
	//serialization function
	virtual void Serialize(FArchive& Ar) override;
	//Data evaluation function of skill Asset
	
virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#if WITH_EDITOR

	virtual UClass* GetBlueprintClass() const override;
	
	//// UBlueprint interface
	virtual bool SupportedByDefaultBlueprintFactory() const override
	{
		return false;
	}

	virtual bool IsValidForBytecodeOnlyRecompile() const override { return false; }
	virtual bool CanRecompileWhilePlayingInEditor() const override;
	virtual void PostLoad() override;
#endif	// #if WITH_EDITOR
};
