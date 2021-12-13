// Fill out your copyright notice in the Description page of Project Settings.


#include "SKAUEdGraphTab.h"

#include "SKAUEdGraph.h"
#include "SKAUEdGraphSchema.h"


void SKAUEdGraphTab::Construct(const FArguments& InArgs)
{
	//Add the graph object containing all nodes
	GraphObj = NewObject<USKAUEdGraph>();
	GraphObj->Schema = USKAUEdGraphSchema::StaticClass();
	GraphObj->AddToRoot();
	
	//make the new editor of Grapheditor
	GraphEditorPtr = SNew(SGraphEditor)
		.GraphToEdit(GraphObj);
		

	//give the ui contents
	ChildSlot
	[
		GraphEditorPtr.ToSharedRef()
	];
	
}

SKAUEdGraphTab::SKAUEdGraphTab()
{
}

SKAUEdGraphTab::~SKAUEdGraphTab()
{
}
