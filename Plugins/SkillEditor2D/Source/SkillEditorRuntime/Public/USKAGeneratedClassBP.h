// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "USKAGeneratedClassBP.generated.h"

/**
 * The class actually stores all data reflection data
 */
UCLASS()
class SKILLEDITORRUNTIME_API USKAGeneratedClassBP : public UBlueprintGeneratedClass//, public IAnimClassInterface
{
	GENERATED_UCLASS_BODY()

public:
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
	// UStruct interface
	virtual void Link(FArchive& Ar, bool bRelinkExistingProperties) override;
	// End of UStruct interface
	// UClass interface
	virtual void PurgeClass(bool bRecompilingOnLoad) override;
	virtual void PostLoadDefaultObject(UObject* Object) override;
	// End of UClass interface
};


