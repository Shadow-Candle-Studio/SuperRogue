#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SKILLEDITORRENDERER_API FSkillEditorRendererModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};