// Copyright Epic Games, Inc. All Rights Reserved.

#include "ULevelSequenceEditorBlueprintLibrary_SKA.h"

#include "ISequencer.h"
#include "IKeyArea.h"
#include "LevelSequence.h"

#include "Modules/ModuleManager.h"
#include "LevelEditor.h"
#include "Subsystems/AssetEditorSubsystem.h"

#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneChannelProxy.h"

namespace
{
	static TWeakPtr<ISequencer> CurrentSequencer;
}

bool ULevelSequenceEditorBlueprintLibrary_SKA::OpenLevelSequence(ULevelSequence* LevelSequence)
{
	if (LevelSequence)
	{
		return GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(LevelSequence);
	}

	return false;
}

ULevelSequence* ULevelSequenceEditorBlueprintLibrary_SKA::GetCurrentLevelSequence()
{
	if (CurrentSequencer.IsValid())
	{
		return Cast<ULevelSequence>(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
	}
	return nullptr;
}

void ULevelSequenceEditorBlueprintLibrary_SKA::CloseLevelSequence()
{
	if (CurrentSequencer.IsValid())
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseAllEditorsForAsset(CurrentSequencer.Pin()->GetRootMovieSceneSequence());
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::Play()
{
	const bool bTogglePlay = false;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->OnPlay(bTogglePlay);
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::Pause()
{
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->Pause();
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::SetCurrentTime(int32 NewFrame)
{
	if (CurrentSequencer.IsValid())
	{
		FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
		FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

		CurrentSequencer.Pin()->SetGlobalTime(ConvertFrameTime(NewFrame, DisplayRate, TickResolution));
	}
}

int32 ULevelSequenceEditorBlueprintLibrary_SKA::GetCurrentTime()
{
	if (CurrentSequencer.IsValid())
	{
		FFrameRate DisplayRate = CurrentSequencer.Pin()->GetFocusedDisplayRate();
		FFrameRate TickResolution = CurrentSequencer.Pin()->GetFocusedTickResolution();

		return ConvertFrameTime(CurrentSequencer.Pin()->GetGlobalTime().Time, TickResolution, DisplayRate).FloorToFrame().Value;
	}
	return 0;
}

bool ULevelSequenceEditorBlueprintLibrary_SKA::IsPlaying()
{
	if (CurrentSequencer.IsValid())
	{
		return CurrentSequencer.Pin()->GetPlaybackStatus() == EMovieScenePlayerStatus::Playing;
	}
	return false;
}

TArray<UMovieSceneTrack*> ULevelSequenceEditorBlueprintLibrary_SKA::GetSelectedTracks()
{
	TArray<UMovieSceneTrack*> OutSelectedTracks;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->GetSelectedTracks(OutSelectedTracks);
	}
	return OutSelectedTracks;
}

TArray<UMovieSceneSection*> ULevelSequenceEditorBlueprintLibrary_SKA::GetSelectedSections()
{
	TArray<UMovieSceneSection*> OutSelectedSections;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->GetSelectedSections(OutSelectedSections);
	}
	return OutSelectedSections;
}

TArray<FSequencerChannelProxy_SKA> ULevelSequenceEditorBlueprintLibrary_SKA::GetSelectedChannels()
{
	TArray<FSequencerChannelProxy_SKA> OutSelectedChannels;
	if (CurrentSequencer.IsValid())
	{
		TArray<const IKeyArea*> SelectedKeyAreas;

		CurrentSequencer.Pin()->GetSelectedKeyAreas(SelectedKeyAreas);

		for (const IKeyArea* KeyArea : SelectedKeyAreas)
		{
			if (KeyArea)
			{
				FSequencerChannelProxy_SKA ChannelProxy(KeyArea->GetName(), KeyArea->GetOwningSection());
				OutSelectedChannels.Add(ChannelProxy);
			}
		}
	}
	return OutSelectedChannels;
}

TArray<UMovieSceneFolder*> ULevelSequenceEditorBlueprintLibrary_SKA::GetSelectedFolders()
{
	TArray<UMovieSceneFolder*> OutSelectedFolders;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->GetSelectedFolders(OutSelectedFolders);
	}
	return OutSelectedFolders;
}

TArray<FGuid> ULevelSequenceEditorBlueprintLibrary_SKA::GetSelectedObjects()
{
	TArray<FGuid> OutSelectedGuids;
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->GetSelectedObjects(OutSelectedGuids);
	}
	return OutSelectedGuids;
}

void ULevelSequenceEditorBlueprintLibrary_SKA::SelectTracks(const TArray<UMovieSceneTrack*>& Tracks)
{
	if (CurrentSequencer.IsValid())
	{
		for (UMovieSceneTrack* Track : Tracks)
		{
			CurrentSequencer.Pin()->SelectTrack(Track);
		}
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::SelectSections(const TArray<UMovieSceneSection*>& Sections)
{
	if (CurrentSequencer.IsValid())
	{
		for (UMovieSceneSection* Section : Sections)
		{
			CurrentSequencer.Pin()->SelectSection(Section);
		}
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::SelectChannels(const TArray<FSequencerChannelProxy_SKA>& Channels)
{
	if (CurrentSequencer.IsValid())
	{
		for (FSequencerChannelProxy_SKA ChannelProxy : Channels)
		{
			UMovieSceneSection* Section = ChannelProxy.Section;
			if (Section)
			{
				TArray<FName> ChannelNames;
				ChannelNames.Add(ChannelProxy.ChannelName);
				CurrentSequencer.Pin()->SelectByChannels(Section, ChannelNames, false, true);
			}
		}
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::SelectFolders(const TArray<UMovieSceneFolder*>& Folders)
{
	if (CurrentSequencer.IsValid())
	{
		for (UMovieSceneFolder* Folder : Folders)
		{
			CurrentSequencer.Pin()->SelectFolder(Folder);
		}
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::SelectObjects(TArray<FGuid> ObjectBindings)
{
	if (CurrentSequencer.IsValid())
	{
		for (FGuid ObjectBinding : ObjectBindings)
		{
			CurrentSequencer.Pin()->SelectObject(ObjectBinding);
		}
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::EmptySelection()
{
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->EmptySelection();
	}
}

void ULevelSequenceEditorBlueprintLibrary_SKA::SetSequencer(TSharedRef<ISequencer> InSequencer)
{
	CurrentSequencer = TWeakPtr<ISequencer>(InSequencer);
}

void ULevelSequenceEditorBlueprintLibrary_SKA::RefreshCurrentLevelSequence()
{
	if (CurrentSequencer.IsValid())
	{
		CurrentSequencer.Pin()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
	}
}
	
TArray<UObject*> ULevelSequenceEditorBlueprintLibrary_SKA::GetBoundObjects(FMovieSceneObjectBindingID ObjectBinding)
{
	TArray<UObject*> BoundObjects;
	if (CurrentSequencer.IsValid())
	{
		for (TWeakObjectPtr<> WeakObject : CurrentSequencer.Pin()->FindBoundObjects(ObjectBinding.GetGuid(), ObjectBinding.GetSequenceID()))
		{
			if (WeakObject.IsValid())
			{
				BoundObjects.Add(WeakObject.Get());
			}
		}

	}
	return BoundObjects;
}


bool ULevelSequenceEditorBlueprintLibrary_SKA::IsLevelSequenceLocked()
{
	if (CurrentSequencer.IsValid())
	{
		TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();
		UMovieSceneSequence* FocusedMovieSceneSequence = Sequencer->GetFocusedMovieSceneSequence();
		if (FocusedMovieSceneSequence) 
		{
			if (FocusedMovieSceneSequence->GetMovieScene()->IsReadOnly()) 
			{
				return true;
			}
			else
			{
				TArray<UMovieScene*> DescendantMovieScenes;
				MovieSceneHelpers::GetDescendantMovieScenes(Sequencer->GetFocusedMovieSceneSequence(), DescendantMovieScenes);

				for (UMovieScene* DescendantMovieScene : DescendantMovieScenes)
				{
					if (DescendantMovieScene && DescendantMovieScene->IsReadOnly())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void ULevelSequenceEditorBlueprintLibrary_SKA::SetLockLevelSequence(bool bLock)
{
	if (CurrentSequencer.IsValid())
	{
		TSharedPtr<ISequencer> Sequencer = CurrentSequencer.Pin();

		if (Sequencer->GetFocusedMovieSceneSequence())
		{
			UMovieScene* MovieScene = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();

			if (bLock != MovieScene->IsReadOnly()) 
			{
				MovieScene->Modify();
				MovieScene->SetReadOnly(bLock);
			}

			TArray<UMovieScene*> DescendantMovieScenes;
			MovieSceneHelpers::GetDescendantMovieScenes(Sequencer->GetFocusedMovieSceneSequence(), DescendantMovieScenes);

			for (UMovieScene* DescendantMovieScene : DescendantMovieScenes)
			{
				if (DescendantMovieScene && bLock != DescendantMovieScene->IsReadOnly())
				{
					DescendantMovieScene->Modify();
					DescendantMovieScene->SetReadOnly(bLock);
				}
			}

			Sequencer->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::Unknown);
		}
	}
}

