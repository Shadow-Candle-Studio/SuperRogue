// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "SkillAssetFactoryFromBinary.generated.h"

/**
 * 
 */
UCLASS()
class SKILLEDITORRUNTIME_API USkillAssetFactoryFromBinary : public UFactory
{
	GENERATED_BODY()
public:
	USkillAssetFactoryFromBinary(const FObjectInitializer& ObjectInitializer);
	virtual UObject* FactoryCreateBinary(UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		UObject* Context,
		const TCHAR* Type,
		const uint8*& Buffer,
		const uint8* BufferEnd,
		FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateFile(UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		const FString& Filename,
		const TCHAR* Parms,
		FFeedbackContext* Warn,
		bool& bOutOperationCanceled) override;
};
