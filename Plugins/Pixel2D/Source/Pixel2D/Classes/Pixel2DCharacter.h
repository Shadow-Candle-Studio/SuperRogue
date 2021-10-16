// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Character.h"
#include "Pixel2DCharacter.generated.h"

// APixel2DCharacter behaves like ACharacter, but uses a UPixel2DComponent instead of a USkeletalMeshComponent as a visual representation
// Note: The variable named Mesh will not be set up on this actor!
UCLASS()
class PIXEL2D_API APixel2DCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	// Name of the Sprite component
	static FName PixelComponentName;

private:
	/** The main skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPixel2DComponent* PixelComponent;
public:

	virtual void PostInitializeComponents() override;

	/** Returns Sprite subobject **/
	FORCEINLINE class UPixel2DComponent* GetSprite() const { return PixelComponent; }
};
