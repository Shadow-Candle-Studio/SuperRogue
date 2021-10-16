 // Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pixel2DComponent.h"

FName APixel2DCharacter::PixelComponentName(TEXT("Pixel0"));

//////////////////////////////////////////////////////////////////////////
// APixel2DCharacter

APixel2DCharacter::APixel2DCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName))
{
	// Try to create the sprite component
	PixelComponent = CreateOptionalDefaultSubobject<UPixel2DComponent>(APixel2DCharacter::PixelComponentName);
	if (PixelComponent)
	{
		PixelComponent->AlwaysLoadOnClient = true;
		PixelComponent->AlwaysLoadOnServer = true;
		PixelComponent->bOwnerNoSee = false;
		PixelComponent->bAffectDynamicIndirectLighting = true;
		PixelComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		PixelComponent->SetupAttachment(GetCapsuleComponent());
		static FName CollisionProfileName(TEXT("CharacterAnimInstance"));
		PixelComponent->SetCollisionProfileName(CollisionProfileName);
		PixelComponent->SetGenerateOverlapEvents(false);
	}
}

void APixel2DCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsPendingKill())
	{
		if (PixelComponent)
		{
			// force animation tick after movement component updates
			if (PixelComponent->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
			{
				PixelComponent->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
			}
		}
	}
}
