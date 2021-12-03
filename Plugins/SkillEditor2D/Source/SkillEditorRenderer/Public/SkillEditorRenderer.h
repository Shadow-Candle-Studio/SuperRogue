#pragma once

#include "CoreMinimal.h"
#include "FSkillEditorShaderRenderer.h"
#include "FSkillEditorViewPortRenderingClient.h"
#include "Modules/ModuleManager.h"

class SKILLEDITORRENDERER_API FSkillEditorRenderer : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	

};
