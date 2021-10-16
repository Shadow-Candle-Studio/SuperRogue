 // Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"

class UEdGraphPin;

struct FPixel2DAnimBlueprintNodeOptionalPinManager : public FOptionalPinManager
{
protected:
	class UPixel2DAnimGraphNode_Base* BaseNode;
	TArray<UEdGraphPin*>* OldPins;

	TMap<FName, UEdGraphPin*> OldPinMap;

public:
	FPixel2DAnimBlueprintNodeOptionalPinManager(class UPixel2DAnimGraphNode_Base* Node, TArray<UEdGraphPin*>* InOldPins);

	/** FOptionalPinManager interface */
	virtual void GetRecordDefaults(FProperty* TestProperty, FOptionalPinFromProperty& Record) const override;
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex, FProperty* Property) const override;
	virtual void PostInitNewPin(UEdGraphPin* Pin, FOptionalPinFromProperty& Record, int32 ArrayIndex, FProperty* Property, uint8* PropertyAddress, uint8* DefaultPropertyAddress) const override;
	virtual void PostRemovedOldPin(FOptionalPinFromProperty& Record, int32 ArrayIndex, FProperty* Property, uint8* PropertyAddress, uint8* DefaultPropertyAddress) const override;
	/** FOptionalPinManager interface */

	void AllocateDefaultPins(UStruct* SourceStruct, uint8* StructBasePtr, uint8* DefaultsPtr);
};
