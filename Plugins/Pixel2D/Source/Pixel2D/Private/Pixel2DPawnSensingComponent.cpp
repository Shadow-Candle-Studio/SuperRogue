// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DPawnSensingComponent.h"
#include "EngineGlobals.h"
#include "TimerManager.h"
#include "CollisionQueryParams.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

DECLARE_CYCLE_STAT(TEXT("Sensing"), STAT_AI_Sensing, STATGROUP_AI);

UPixel2DPawnSensingComponent::UPixel2DPawnSensingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LOSHearingThreshold = 2800.f;
	HearingThreshold = 1400.0f;
	SightRadius = 5000.0f;
	PeripheralVisionAngle = 90.f;
	PeripheralVisionCosine = FMath::Cos(FMath::DegreesToRadians(PeripheralVisionAngle));

	SensingInterval = 0.5f;

	bOnlySensePlayers = true;
	bSeePawns = true;

	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	bAutoActivate = false;

	bEnableSensingUpdates = true;

	MaxSightDistance = 250;
	MinSightDistance = 100;

	SetStartAngle(0.f);
	SetEndAngle(90.f);

}


void UPixel2DPawnSensingComponent::SetPeripheralVisionAngle(const float NewPeripheralVisionAngle)
{
	PeripheralVisionAngle = NewPeripheralVisionAngle;
	PeripheralVisionCosine = FMath::Cos(FMath::DegreesToRadians(PeripheralVisionAngle));
}


void UPixel2DPawnSensingComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetPeripheralVisionAngle(PeripheralVisionAngle);

	// Update internal state/variables
	SetStartAngle(StartAngle);
	SetEndAngle(EndAngle);

	if (bEnableSensingUpdates)
	{
		bEnableSensingUpdates = false; // force an update
		SetSensingUpdatesEnabled(true);
	}
}


void UPixel2DPawnSensingComponent::SetSensingUpdatesEnabled(const bool bEnabled)
{
	if (bEnableSensingUpdates != bEnabled)
	{
		bEnableSensingUpdates = bEnabled;

		if (bEnabled && SensingInterval > 0.f)
		{
			// Stagger initial updates so all sensors do not update at the same time (to avoid hitches).
			const float InitialDelay = (SensingInterval * FMath::SRand()) + KINDA_SMALL_NUMBER;
			SetTimer(InitialDelay);
		}
		else
		{
			SetTimer(0.f);
		}
	}
}


void UPixel2DPawnSensingComponent::SetTimer(const float TimeInterval)
{
	// Only necessary to update if we are the server
	AActor* const Owner = GetOwner();
	if (IsValid(Owner) && GEngine->GetNetMode(GetWorld()) < NM_Client)
	{
		Owner->GetWorldTimerManager().SetTimer(TimerHandle_OnTimer, this, &UPixel2DPawnSensingComponent::OnTimer, TimeInterval, false);
	}
}


void UPixel2DPawnSensingComponent::SetSensingInterval(const float NewSensingInterval)
{
	if (SensingInterval != NewSensingInterval)
	{
		SensingInterval = NewSensingInterval;

		AActor* const Owner = GetOwner();
		if (IsValid(Owner))
		{
			if (SensingInterval <= 0.f)
			{
				SetTimer(0.f);
			}
			else if (bEnableSensingUpdates)
			{
				float CurrentElapsed = Owner->GetWorldTimerManager().GetTimerElapsed(TimerHandle_OnTimer);
				CurrentElapsed = FMath::Max(0.f, CurrentElapsed);

				if (CurrentElapsed < SensingInterval)
				{
					// Extend lifetime by remaining time.
					SetTimer(SensingInterval - CurrentElapsed);
				}
				else if (CurrentElapsed > SensingInterval)
				{
					// Basically fire next update, because time has already expired.
					// Don't want to fire immediately in case an update tries to change the interval, looping endlessly.
					SetTimer(KINDA_SMALL_NUMBER);
				}
			}
		}
	}
}


void UPixel2DPawnSensingComponent::OnTimer()
{
	SCOPE_CYCLE_COUNTER(STAT_AI_Sensing);

	AActor* const Owner = GetOwner();
	if (!IsValid(Owner) || !IsValid(Owner->GetWorld()))
	{
		return;
	}

	if (CanSenseAnything())
	{
		UpdateAISensing();
	}

	SetTimer(SensingInterval);
};


AActor* UPixel2DPawnSensingComponent::GetSensorActor() const
{
	AActor* SensorActor = GetOwner();
	AController* Controller = Cast<AController>(SensorActor);
	if (IsValid(Controller))
	{
		// In case the owner is a controller, use the controlled pawn as the sensing location.
		SensorActor = Controller->GetPawn();
	}

	if (!IsValid(SensorActor))
	{
		return NULL;
	}

	return SensorActor;
}

AController* UPixel2DPawnSensingComponent::GetSensorController() const
{
	AActor* SensorActor = GetOwner();
	AController* Controller = Cast<AController>(SensorActor);

	if (IsValid(Controller))
	{
		return Controller;
	}
	else
	{
		APawn* Pawn = Cast<APawn>(SensorActor);
		if (IsValid(Pawn) && IsValid(Pawn->Controller))
		{
			return Pawn->Controller;
		}
	}

	return NULL;
}

bool UPixel2DPawnSensingComponent::IsSensorActor(const AActor* Actor) const
{
	return (Actor == GetSensorActor());
}

bool UPixel2DPawnSensingComponent::HasLineOfSightTo(const AActor* Other) const
{
	AController* SensorController = GetSensorController();
	if (SensorController == NULL)
	{
		return false;
	}

	return SensorController->LineOfSightTo(Other, FVector::ZeroVector, true);
}

bool UPixel2DPawnSensingComponent::CanSenseAnything() const
{
	return (bSeePawns && OnSeePawn.IsBound());
}


void UPixel2DPawnSensingComponent::UpdateAISensing()
{
	AActor* const Owner = GetOwner();
	check(IsValid(Owner));
	check(IsValid(Owner->GetWorld()));

	if (bOnlySensePlayers)
	{
		for (FConstPlayerControllerIterator Iterator = Owner->GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PC = Iterator->Get();
			if (IsValid(PC))
			{
				APawn* Pawn = PC->GetPawn();
				if (IsValid(Pawn) && !IsSensorActor(Pawn))
				{
					SensePawn(*Pawn);
				}
			}
		}
	}
	else
	{
		for (TActorIterator<APawn> It(Owner->GetWorld()); It; ++It)
		{
			APawn* Pawn = *It;
			if (IsValid(Pawn) && !IsSensorActor(Pawn))
			{
				SensePawn(*Pawn);
			}
		}
	}
}


void UPixel2DPawnSensingComponent::SensePawn(APawn& Pawn)
{
	// Visibility checks
	bool bHasSeenPawn = false;
	bool bHasFailedLineOfSightCheck = false;
	if (bSeePawns && ShouldCheckVisibilityOf(&Pawn))
	{
		if (CouldSeePawn(&Pawn, true))
		{
			if (HasLineOfSightTo(&Pawn))
			{
				BroadcastOnSeePawn(Pawn);
				bHasSeenPawn = true;
			}
			else
			{
				bHasFailedLineOfSightCheck = true;
			}
		}
	}

	// Sound checks
	// No need to 'hear' something if you've already seen it!
	if (bHasSeenPawn)
	{
		return;
	}
}


void UPixel2DPawnSensingComponent::BroadcastOnSeePawn(APawn& Pawn)
{
	OnSeePawn.Broadcast(&Pawn);
}


bool UPixel2DPawnSensingComponent::CouldSeePawn(const APawn *Other, bool bMaySkipChecks) const
{
	if (!Other)
	{
		return false;
	}

	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	FVector const OtherLoc = Other->GetActorLocation();
	FVector const SensorLoc = GetSensorLocation();
	FVector const SelfToOther = OtherLoc - SensorLoc;

	// check max sight distance
	float const SelfToOtherDistSquared = SelfToOther.SizeSquared();
	if (SelfToOtherDistSquared > FMath::Square(MaxSightDistance))
	{
		return false;
	}

	// check min sight distance
	if (SelfToOtherDistSquared < FMath::Square(MinSightDistance))
	{
		return false;
	}

	// may skip if more than some fraction of maxdist away (longer time to acquire)
	if (bMaySkipChecks && (FMath::Square(FMath::FRand()) * SelfToOtherDistSquared > FMath::Square(0.4f * SightRadius)))
	{
		return false;
	}

	// check field of view
	FVector const SelfToOtherDir = SelfToOther.GetSafeNormal();
	FVector const MyFacingDir = GetSensorRotation().Vector();
	const float SelfToOtherCos = ((SelfToOtherDir | MyFacingDir));
	const float SelfToOtherSin = ((SelfToOtherDir ^ MyFacingDir)).Size();
	const float SelfToOtherAngleRad = FMath::Atan2(SelfToOtherSin, SelfToOtherCos);

	const float MinAngleRad = FMath::Min(StartAngleRad, EndAngleRad);
	const float MaxAngleRad = FMath::Max(StartAngleRad, EndAngleRad);

	float Start = FMath::RadiansToDegrees(StartAngleRad);
	float End = FMath::RadiansToDegrees(EndAngleRad);
	float Mid = FMath::RadiansToDegrees(SelfToOtherAngleRad);
	//-pi/2-1 +pi/2

	End = (End - Start) < 0.0f ? End - Start + 360.0f : End - Start;
	Mid = (Mid - Start) < 0.0f ? Mid - Start + 360.0f : Mid - Start;

	return (Mid < End);
}

FVector UPixel2DPawnSensingComponent::GetSensorLocation() const
{
	FVector SensorLocation(FVector::ZeroVector);
	const AActor* SensorActor = GetSensorActor();

	if (SensorActor != NULL)
	{
		FRotator ViewRotation;
		SensorActor->GetActorEyesViewPoint(SensorLocation, ViewRotation);
	}

	return SensorLocation;
}

FRotator UPixel2DPawnSensingComponent::GetSensorRotation() const
{
	FRotator SensorRotation(FRotator::ZeroRotator);
	const AActor* SensorActor = GetSensorActor();

	if (SensorActor != NULL)
	{
		SensorRotation = SensorActor->GetActorRotation();
	}

	return SensorRotation;
}

#if WITH_EDITOR
void UPixel2DPawnSensingComponent::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == FName(TEXT("StartAngle")))
	{
		StartAngleCosine = FMath::Cos(FMath::DegreesToRadians(StartAngle));
		StartAngleSine = FMath::Sin(FMath::DegreesToRadians(StartAngle));
		StartAngleRad = FMath::DegreesToRadians(FMath::UnwindDegrees(StartAngle));
	}
	if (PropertyChangedEvent.GetPropertyName() == FName(TEXT("EndAngle")))
	{
		EndAngleCosine = FMath::Cos(FMath::DegreesToRadians(EndAngle));
		EndAngleSine = FMath::Sin(FMath::DegreesToRadians(EndAngle));
		EndAngleRad = FMath::DegreesToRadians(FMath::UnwindDegrees(EndAngle));
	}

}
#endif

bool UPixel2DPawnSensingComponent::ShouldCheckVisibilityOf(APawn *Pawn) const
{
	const bool bPawnIsPlayer = (Pawn->Controller && Pawn->Controller->PlayerState);
	if (!bSeePawns || (bOnlySensePlayers && !bPawnIsPlayer))
	{
		return false;
	}

	if (Pawn->IsHidden())
	{
		return false;
	}

	return true;
}

void UPixel2DPawnSensingComponent::SetStartAngle(float NewAngle)
{
	StartAngle = NewAngle;
	StartAngleCosine = FMath::Cos(FMath::DegreesToRadians(StartAngle));
	StartAngleSine = FMath::Sin(FMath::DegreesToRadians(StartAngle));
	StartAngleRad = FMath::DegreesToRadians(FMath::UnwindDegrees(StartAngle));
}

void UPixel2DPawnSensingComponent::SetEndAngle(float NewAngle)
{
	EndAngle = NewAngle;
	EndAngleCosine = FMath::Cos(FMath::DegreesToRadians(EndAngle));
	EndAngleSine = FMath::Sin(FMath::DegreesToRadians(EndAngle));
	EndAngleRad = FMath::DegreesToRadians(FMath::UnwindDegrees(EndAngle));
}


