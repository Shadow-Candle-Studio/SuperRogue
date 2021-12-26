// Fill out your copyright notice in the Description page of Project Settings.


#include "SKACompilerContext.h"

#include "KismetReinstanceUtilities.h"
#include "USKAInstance.h"


SKACompilerContext::SKACompilerContext(USkillAsset* SourceSketch,
                                       FCompilerResultsLog& InMessageLog,
                                       const FKismetCompilerOptions& InCompileOptions):
FKismetCompilerContext(SourceSketch,InMessageLog,InCompileOptions),SkillAsset(SourceSketch)
{
}

SKACompilerContext::~SKACompilerContext()
{
}

void SKACompilerContext::PostCompile()
{
	FKismetCompilerContext::PostCompile();
	USKAGeneratedClassBP* AnimBlueprintGeneratedClass = CastChecked<USKAGeneratedClassBP>(NewClass);
	USKAInstance* DefaultUSKAInstance = CastChecked<USKAInstance>(AnimBlueprintGeneratedClass->GetDefaultObject());
	
}
void SKACompilerContext::CreateFunctionList()
{
	FKismetCompilerContext::CreateFunctionList();
}

void SKACompilerContext::CreateClassVariablesFromBlueprint()
{
	FKismetCompilerContext::CreateClassVariablesFromBlueprint();
}



void SKACompilerContext::MergeUbergraphPagesIn(UEdGraph* Ubergraph)
{
	Super::MergeUbergraphPagesIn(Ubergraph);
}

void SKACompilerContext::ProcessOneFunctionGraph(UEdGraph* SourceGraph, bool bInternalFunction)
{
	Super::ProcessOneFunctionGraph(SourceGraph, bInternalFunction);
}



void SKACompilerContext::SpawnNewClass(const FString& NewClassName)
{
	//FKismetCompilerContext::SpawnNewClass(NewClassName);


	SkillAssetGeneratedClassBP = FindObject<USKAGeneratedClassBP>(Blueprint->GetOutermost(), *NewClassName);

	if (SkillAssetGeneratedClassBP == nullptr)
	{
		SkillAssetGeneratedClassBP = NewObject<USKAGeneratedClassBP>(Blueprint->GetOutermost(), FName(*NewClassName), RF_Public | RF_Transactional);
	}
	else
	{
		// Already existed, but wasn't linked in the Blueprint yet due to load ordering issues
		FBlueprintCompileReinstancer::Create(SkillAssetGeneratedClassBP);
	}
	NewClass = SkillAssetGeneratedClassBP;
}

void SKACompilerContext::OnNewClassSet(UBlueprintGeneratedClass* ClassToUse)
{
	SkillAssetGeneratedClassBP=CastChecked<USKAGeneratedClassBP>(ClassToUse);
	
}

void SKACompilerContext::CopyTermDefaultsToDefaultObject(UObject* DefaultObject)
{
	Super::CopyTermDefaultsToDefaultObject(DefaultObject);
}

void SKACompilerContext::PostCompileDiagnostics()
{
	FKismetCompilerContext::PostCompileDiagnostics();
	
}



void SKACompilerContext::EnsureProperGeneratedClass(UClass*& TargetClass)
{
	FKismetCompilerContext::EnsureProperGeneratedClass(TargetClass);
	if (TargetClass && !((UObject*)TargetClass)->IsA(USKAGeneratedClassBP::StaticClass()))
	{
		FKismetCompilerUtilities::ConsignToOblivion(TargetClass, Blueprint->bIsRegeneratingOnLoad);
		TargetClass = NULL;
	}
}

void SKACompilerContext::CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOldCDO)
{
	Super::CleanAndSanitizeClass(ClassToClean, InOldCDO);
	// Make sure our typed pointer is set
	check(ClassToClean == NewClass && SkillAssetGeneratedClassBP == NewClass);

	// Reset the baked data
	//REVISIT: Move this into PurgeClass
	//    NewAnimBlueprintClass->BakedStateMachines.Empty();
	//    NewAnimBlueprintClass->AnimNotifies.Empty();

	// SkillAssetGeneratedClassBP->RootAnimNodeIndex = INDEX_NONE;
	// SkillAssetGeneratedClassBP->RootAnimNodeProperty = NULL;
	// SkillAssetGeneratedClassBP->AnimNodeProperties.Empty();
}

void SKACompilerContext::FinishCompilingClass(UClass* Class)
{
	Super::FinishCompilingClass(Class);
}
