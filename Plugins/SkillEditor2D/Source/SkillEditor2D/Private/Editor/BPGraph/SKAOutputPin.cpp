// Fill out your copyright notice in the Description page of Project Settings.


#include "SKAOutputPin.h"


void SKAOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor( EMouseCursor::Default );

	typedef SKAOutputPin ThisClass;

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct( SBorder::FArguments()
		.BorderImage( this, &SKAOutputPin::GetPinBorder )
		.BorderBackgroundColor( this, &ThisClass::GetPinColor )
		.OnMouseButtonDown( this, &ThisClass::OnPinMouseDown )
		.Cursor( this, &ThisClass::GetPinCursor )
	);
}

TSharedRef<SWidget>	SKAOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}



const FSlateBrush* SKAOutputPin::GetPinBorder() const
{
	return ( IsHovered() )
		? FEditorStyle::GetBrush( TEXT("Graph.StateNode.Pin.BackgroundHovered") )
		: FEditorStyle::GetBrush( TEXT("Graph.StateNode.Pin.Background") );
}
