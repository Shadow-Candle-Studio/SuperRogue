// Fill out your copyright notice in the Description page of Project Settings.


#include "ULevelSequenceEditorSettings_SKA.h"


ULevelSequenceEditorSettings_SKA::ULevelSequenceEditorSettings_SKA(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoBindToSimulate = true;
	bAutoBindToPIE      = true;
}

ULevelSequenceMasterSequenceSettings_SKA::ULevelSequenceMasterSequenceSettings_SKA(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MasterSequenceName(TEXT("Sequence"))
	, MasterSequenceSuffix(TEXT("Master"))
	, MasterSequenceNumShots(5)
{
	MasterSequenceBasePath.Path = TEXT("/Game/Cinematics/Sequences");
}
