// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KismetCompiler.h"
#include "SKAUEdGraphSchema.h"
#include "SkillAsset.h"
#include "USKAGeneratedClassBP.h"
#include "Animation/AnimNodeBase.h"
/**
 * SKACompilerContext
 *
 * This class contains functionalities used to compile the blueprint, so that other blueprints can safely use the blueprint to run scripts
 */

class SKILLEDITOR2D_API SKACompilerContext : public FKismetCompilerContext
{
protected:
	typedef FKismetCompilerContext Super;
public:
	SKACompilerContext(USkillAsset* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompileOptions);
	virtual ~SKACompilerContext();

	virtual void PostCompile() override;

protected:
	// Implementation of FKismetCompilerContext interface
	virtual void CreateClassVariablesFromBlueprint() override;
	virtual UEdGraphSchema_K2* CreateSchema() override;
	virtual void MergeUbergraphPagesIn(UEdGraph* Ubergraph) override;
	virtual void ProcessOneFunctionGraph(UEdGraph* SourceGraph, bool bInternalFunction = false) override;
	virtual void CreateFunctionList() override;
	virtual void SpawnNewClass(const FString& NewClassName) override;
	virtual void OnNewClassSet(UBlueprintGeneratedClass* ClassToUse) override;
	virtual void CopyTermDefaultsToDefaultObject(UObject* DefaultObject) override;
	virtual void PostCompileDiagnostics() override;
	virtual void EnsureProperGeneratedClass(UClass*& TargetClass) override;
	virtual void CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOldCDO) override;
	virtual void FinishCompilingClass(UClass* Class) override;
	// End of FKismetCompilerContext interface
	protected:
	typedef TArray<UEdGraphPin*> UEdGraphPinArray;
protected:
	USKAGeneratedClassBP* SkillAssetGeneratedClassBP;
	USkillAsset* SkillAsset;
	USKAUEdGraphSchema* SKASchema;
};