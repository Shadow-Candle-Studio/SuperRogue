#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SkillEditorWindowStyle.h"

class FSkillEditorcommands : public TCommands<FSkillEditorcommands>
{
public:
	FSkillEditorcommands(): TCommands<FSkillEditorcommands>(TEXT("SkillEditor2D"),
		NSLOCTEXT("Contexts","SkillEditor2D","SkillEditor2D Plugin"),
		NAME_None,
		SkillEditorWindowStyle::GetStyleSetName())
	{}
	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenPluginWindow;
	TSharedPtr<FUICommandInfo> Textfunc;
};
