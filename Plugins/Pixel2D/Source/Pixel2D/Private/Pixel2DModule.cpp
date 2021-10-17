// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"

#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "Pixel2DAnim"

//////////////////////////////////////////////////////////////////////////
// FPixel2DAnimModule

class FPixel2DAnimModule : public IModuleInterface
{
public:
	FPixel2DAnimModule()
	{}


private:


public:
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}


private:
};

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_MODULE(FPixel2DAnimModule, Pixel2D);

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
