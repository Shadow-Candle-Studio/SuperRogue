// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "Pixel2DAnimInstance.h"
#include "Pixel2DComponent.h"
#include "Pixel2DActor.generated.h"

/**
* An instance of a UPixel2DAnimInstance in a level.
*
* This actor is created when you drag a UPixel2DAnimInstance asset from the content browser into the level, and
* it is just a thin wrapper around a UPixel2DComponent that actually references the asset.
*/
UCLASS(ComponentWrapperClass, Blueprintable)
class PIXEL2D_API APixel2DActor : public AActor
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(Category = Sprite, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Flipbook", AllowPrivateAccess = "true"))
	class UPixel2DComponent* RenderComponent;

public:

	// AActor interface
#if WITH_EDITOR
	virtual bool GetReferencedContentObjects(TArray<UObject*>& Objects) const override;
#endif
	// End of AActor interface

	void RegisterAnimInstance(UClass * NewAnimInstance);

	/** Returns RenderComponent subobject **/
	FORCEINLINE class UPixel2DComponent*     GetRenderComponent() const { return RenderComponent; }
};


