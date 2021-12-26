// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "USKAInstance.generated.h"

/**
 * store all of the blueprint editable 
 */

UCLASS(transient, Blueprintable, hideCategories = SKillAssetInstance, BlueprintType, meta = (BlueprintThreadSafe))
class SKILLEDITORRUNTIME_API USKAInstance : public UObject
{
	GENERATED_UCLASS_BODY()

	// Disable compiler-generated deprecation warnings by implementing our own destructor
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	~USKAInstance() {}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	// UPROPERTY(BlueprintReadWrite,EditAnywhere)
	// int NumFromUSKinstande;

	/** Flag to check back on the game thread that indicates we need to run PostUpdateAnimation() in the post-eval call */
	bool bNeedsUpdate;
	public:
		virtual void PostInitProperties() override;

		



};
