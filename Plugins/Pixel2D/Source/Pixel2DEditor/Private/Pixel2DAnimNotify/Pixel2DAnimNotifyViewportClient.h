// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "PaperFlipbook.h"
#include "EditorViewportClient.h"

class FCanvas;
class UPaperFlipbookComponent;

//////////////////////////////////////////////////////////////////////////
// FPixel2DAnimNotifyViewportClient

class FPixel2DAnimNotifyViewportClient : public FEditorViewportClient
{
public:
	/** Constructor */
	FPixel2DAnimNotifyViewportClient(const TAttribute<class UPaperFlipbook*>& InFlipbookBeingEdited);

	// FViewportClient interface
	virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;
	virtual void Tick(float DeltaSeconds) override;
	// End of FViewportClient interface

	// FEditorViewportClient interface
	virtual FLinearColor GetBackgroundColor() const override;
	// End of FEditorViewportClient interface

	UPaperFlipbookComponent* GetPreviewComponent() const
	{
		return AnimatedRenderComponent.Get();
	}

private:
	FBox GetDesiredFocusBounds() const;
	void SetupViewport();
	
private:

	// The preview scene
	FPreviewScene OwnedPreviewScene;

	// The flipbook being displayed in this client
	TAttribute<class UPaperFlipbook*> FlipbookBeingEdited;

	// A cached pointer to the flipbook that was being edited last frame. Used for invalidation reasons.
	TWeakObjectPtr<class UPaperFlipbook> FlipbookBeingEditedLastFrame;

	// Render component for the sprite being edited
	TWeakObjectPtr<UPaperFlipbookComponent> AnimatedRenderComponent;
};