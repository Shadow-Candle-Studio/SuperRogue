// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnrealEd/Classes/Factories/Factory.h"
#include "Runtime/Engine/Classes/Engine/Blueprint.h"
#include "Pixel2DAnimBlueprintFactory.generated.h"

UCLASS()
class UPixel2DAnimBlueprintFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	// The type of blueprint that will be created
	UPROPERTY(EditAnywhere, Category = Pixel2DAnimBlueprintFactory)
	TEnumAsByte<EBlueprintType> BlueprintType;

	// The parent class of the created blueprint
	UPROPERTY(EditAnywhere, Category = Pixel2DAnimBlueprintFactory, meta = (AllowAbstract = ""))
	TSubclassOf<class UPixel2DAnimInstance> ParentClass;

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	// End of UFactory interface
};

