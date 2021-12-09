#include "FSkillEditorcommands.h"
#define LOCTEXT_NAMESPACE "FSkillEditor2DModule"

void FSkillEditorcommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "SkillEditor2D","Open 2d Skill editor window",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(Textfunc, "SkillEditor2D","Test button for this",EUserInterfaceActionType::Button,FInputChord());
	
}
#undef LOCTEXT_NAMESPACE