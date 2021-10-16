// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimBlueprintFactory.h"
#include "Pixel2DAnimBlueprint.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphSchema_K2.h"

#include "Pixel2DAnimInstance.h"
#include "Pixel2DAnimGraph.h"
#include "Pixel2DAnimGraphSchema.h"

#include "AnimationGraph.h"
#include "AnimationGraphSchema.h"

UPixel2DAnimBlueprintFactory::UPixel2DAnimBlueprintFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UPixel2DAnimBlueprint::StaticClass();

	bCreateNew = true;
	bEditorImport = true;
	bEditAfterNew = true;
	ParentClass = UPixel2DAnimInstance::StaticClass();
}

UObject* UPixel2DAnimBlueprintFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(InClass, InParent, InName, Flags, Context, Warn, NAME_None);
}

UObject* UPixel2DAnimBlueprintFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(Class->IsChildOf(UPixel2DAnimBlueprint::StaticClass()));
	UPixel2DAnimBlueprint * SpriteAnimBP = CastChecked<UPixel2DAnimBlueprint>(FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BlueprintType, UPixel2DAnimBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext));

	// Add the pixel anim graph
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(SpriteAnimBP, K2Schema->GN_AnimGraph, UPixel2DAnimGraph::StaticClass(), UPixel2DAnimGraphSchema::StaticClass());
	FBlueprintEditorUtils::AddDomainSpecificGraph(SpriteAnimBP, NewGraph);
	SpriteAnimBP->LastEditedDocuments.Add(NewGraph);
	NewGraph->bAllowDeletion = false;

	// Mark the BP as being regenerated, so it will not be confused as needing to be loaded and regenerated when a referenced BP loads.
	SpriteAnimBP->bHasBeenRegenerated = true;

	return SpriteAnimBP;
}