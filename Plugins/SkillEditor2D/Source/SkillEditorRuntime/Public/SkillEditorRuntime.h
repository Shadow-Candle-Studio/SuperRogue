// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateStyle.h"

#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"



class SKILLEDITORRUNTIME_API FSkillEditorRuntimeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override;
private :
TSharedPtr<FSlateStyleSet> StyleSet;
};
