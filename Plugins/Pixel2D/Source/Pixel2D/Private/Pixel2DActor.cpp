// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DActor.h"
#include "Pixel2DComponent.h"
#include "Pixel2DAnimInstance.h"
#include "PaperFlipbook.h"

//////////////////////////////////////////////////////////////////////////
// APixel2DActor

APixel2DActor::APixel2DActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	RenderComponent = CreateDefaultSubobject<UPixel2DComponent>(TEXT("Pixel2DRenderComponent"));
	RootComponent = RenderComponent;
}

#if WITH_EDITOR
bool APixel2DActor::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
	Super::GetReferencedContentObjects(Objects);

	if (UPaperFlipbook* FlipbookAsset = RenderComponent->GetFlipbook())
	{
		Objects.Add(FlipbookAsset);
	}
	return true;
}
#endif

void APixel2DActor::RegisterAnimInstance(UClass * NewAnimInstance)
{
	RenderComponent->UnregisterComponent();
	RenderComponent->SetAnimInstance(NewAnimInstance);
	RenderComponent->RegisterComponent();
}
