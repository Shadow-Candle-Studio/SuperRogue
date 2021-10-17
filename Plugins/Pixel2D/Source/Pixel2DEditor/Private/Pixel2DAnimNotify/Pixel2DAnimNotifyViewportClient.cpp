// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pixel2DAnimNotifyViewportClient.h"
#include "PaperFlipbookComponent.h"
#include "CanvasItem.h"
#include "Engine/CollisionProfile.h"
#include "Utils.h"

#include "CanvasTypes.h"
#include "AssetEditorModeManager.h"
#include "Paper2DModule.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

//////////////////////////////////////////////////////////////////////////
// FFlipbookEditorViewportClient

FPixel2DAnimNotifyViewportClient::FPixel2DAnimNotifyViewportClient(const TAttribute<UPaperFlipbook*>& InFlipbookBeingEdited)
	: FEditorViewportClient(new FAssetEditorModeManager(), nullptr, nullptr)
{

	SetupViewport();

	FlipbookBeingEdited = InFlipbookBeingEdited;
	FlipbookBeingEditedLastFrame = FlipbookBeingEdited.Get();
	PreviewScene = &OwnedPreviewScene;

	SetRealtime(false);

	// Create a render component for the sprite being edited
	AnimatedRenderComponent = NewObject<UPaperFlipbookComponent>();
	AnimatedRenderComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	AnimatedRenderComponent->SetFlipbook(FlipbookBeingEdited.Get());
	AnimatedRenderComponent->UpdateBounds();
	AnimatedRenderComponent->Stop();
	PreviewScene->AddComponent(AnimatedRenderComponent.Get(), FTransform::Identity);

	DrawHelper.bDrawGrid = false;

	EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetCompositeEditorPrimitives(true);
}

void FPixel2DAnimNotifyViewportClient::SetupViewport()
{
	//@TODO: Pretty lame hardcoding
	//@TODO: Doesn't handle negatives either (not really)
	const bool XX = FMath::IsNearlyEqual(PaperAxisX.X, 1.0f);
	const bool XY = FMath::IsNearlyEqual(PaperAxisX.Y, 1.0f);
	const bool YY = FMath::IsNearlyEqual(PaperAxisY.Y, 1.0f);
	const bool YZ = FMath::IsNearlyEqual(PaperAxisY.Z, 1.0f);

	ELevelViewportType NewViewportType = LVT_OrthoXZ;
	if (XX && YY)
	{
		NewViewportType = LVT_OrthoXY;
	}
	else if (XX && YZ)
	{
		NewViewportType = LVT_OrthoXZ;
	}
	else if (XY && YZ)
	{
		NewViewportType = LVT_OrthoYZ;
	}
	else
	{
		//@TODO: Unsupported axes
	}
	SetViewModes(VMI_Lit, VMI_Lit);
	SetViewportType(NewViewportType);

	// Get the correct general direction of the perspective mode; the distance doesn't matter much as we've queued up a deferred zoom that will calculate a much better distance
	SetInitialViewTransform(LVT_Perspective, -100.0f * PaperAxisZ, PaperAxisZ.Rotation(), 0.0f);
}
void FPixel2DAnimNotifyViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
{
	FEditorViewportClient::DrawCanvas(InViewport, View, Canvas);

	const bool bIsHitTesting = Canvas.IsHitTesting();
	if (!bIsHitTesting)
	{
		Canvas.SetHitProxy(nullptr);
	}

	int32 YPos = 42;

	static const FText FlipbookHelpStr = LOCTEXT("Pixel2DNotifHelp", "Right-click on the timeline to adjust the notification position");

	// Display tool help
	{
		FCanvasTextItem TextItem(FVector2D(6, YPos), FlipbookHelpStr, GEngine->GetSmallFont(), FLinearColor::White);
		TextItem.EnableShadow(FLinearColor::Black);
		TextItem.Draw(&Canvas);
		YPos += 36;
	}
}

FBox FPixel2DAnimNotifyViewportClient::GetDesiredFocusBounds() const
{
	return AnimatedRenderComponent->Bounds.GetBox();
}

void FPixel2DAnimNotifyViewportClient::Tick(float DeltaSeconds)
{
	if (AnimatedRenderComponent.IsValid())
	{
		UPaperFlipbook* Flipbook = FlipbookBeingEdited.Get();
		if (Flipbook != FlipbookBeingEditedLastFrame.Get())
		{
			AnimatedRenderComponent->SetFlipbook(Flipbook);
			AnimatedRenderComponent->UpdateBounds();
			AnimatedRenderComponent->Stop();
			FlipbookBeingEditedLastFrame = Flipbook;
		}

		// Keep focus on the component
		FIntPoint Size = Viewport->GetSizeXY();
		if ((Size.X > 0) && (Size.Y > 0))
		{
			FBox BoundsToFocus = GetDesiredFocusBounds();
			if (ViewportType != LVT_Perspective)
			{
				TGuardValue<ELevelViewportType> SaveViewportType(ViewportType, LVT_Perspective);
				FocusViewportOnBox(BoundsToFocus, true);
			}

			FocusViewportOnBox(BoundsToFocus, true);
		}
	}

	FEditorViewportClient::Tick(DeltaSeconds);

	if (!GIntraFrameDebuggingGameThread)
	{
		OwnedPreviewScene.GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

FLinearColor FPixel2DAnimNotifyViewportClient::GetBackgroundColor() const
{
	return FLinearColor(0.1f, 0.1f, 0.1f);
}


//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
