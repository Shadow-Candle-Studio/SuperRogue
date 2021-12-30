// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "SKABFactory.generated.h"

/**
 * Binary import factory, not really used, you can ignore it
 */
UCLASS()
class SKILLEDITOR2D_API USKABFactory : public UFactory
{
	GENERATED_BODY()
public:
	USKABFactory(const FObjectInitializer& ObjectInitializer);
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
	virtual uint32 GetMenuCategories() const override;
	
};
