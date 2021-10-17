// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"

#include "Pixel2DAnimBlueprint.generated.h"

class UBlueprint;
class FCompilerResultsLog;
struct FKismetCompilerOptions;
class FKismetCompilerContext;

UCLASS(BlueprintType, meta = (DisplayThumbnail = "true"))
class  PIXEL2D_API UPixel2DAnimBlueprint : public UBlueprint
{
	GENERATED_UCLASS_BODY()

	// REVISIT: to be removed
	UPROPERTY(EditAnywhere, Category = Optimization)
	bool bUseMultiThreadedAnimationUpdate;

	virtual void Serialize(FArchive& Ar) override;

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


