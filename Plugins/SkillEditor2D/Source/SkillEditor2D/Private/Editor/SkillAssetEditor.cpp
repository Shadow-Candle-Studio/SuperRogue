// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillAssetEditor.h"

#include "AssetToolsModule.h"
#include "BlueprintEditorUtils.h"
#include "FileHelper.h"
#include "FSkillEditorcommands.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h"
#include "IPluginManager.h"
#include "ObjectTools.h"
#include "SBlueprintEditorToolbar.h"
#include "SkillAsset.h"
#include "SkillAssetEditorAPPMode.h"
#include "SkillEditor2D.h"
#include "ToolMenus.h"
#include "../../../../../Pixel2D/Source/Pixel2D/Classes/Pixel2DCharacter.h"


const FName SkillAssetEditorAppIdentifier = FName(TEXT("SkillAssetEditorApp"));
#define LOCTEXT_NAMESPACE "SkillAssetEditor"

const FName FSkillAssetEditor::ToolkitFName
(TEXT("SkillAssetEditor"));
const FName FSkillAssetEditor::PreviewTabId
(TEXT("SkillAssetEditor_Preview"));
const FName FSkillAssetEditor::GraphCanvasId
(TEXT("SkillAssetEditor_BPGraph"));
const FName FSkillAssetEditor::PropertiesPanelTabID
(TEXT("SkillAssetEditor_Properties"));
const FName FSkillAssetEditor::SequencerAreaTabID
(TEXT("SkillAssetEditor_SequencerArea"));


UBlueprint* FSkillAssetEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<USkillAsset>()) { return (UBlueprint*)EditingObjs[i]; }
	}
	return nullptr;
}



void FSkillAssetEditor::InitSkillAssetEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost, USkillAsset* InSkillAsset)
{
	//set the skill asset
	SetSkillAsset(InSkillAsset);
	//Map the click message to specific function
	SkillAssetExtcommands=MakeShareable(new FUICommandList);
	SkillAssetExtcommands->MapAction(FSkillEditorcommands::Get().Textfunc,
		FExecuteAction::CreateRaw(this,&FSkillAssetEditor::TextFuncOncliked),
		FCanExecuteAction());
	if (!Toolbar.IsValid())
	{
		Toolbar = MakeShareable(new FBlueprintEditorToolbar(SharedThis(this)));
	}
	
	// Build up a list of objects being edited in this asset editor
	TArray<UObject*> ObjectsBeingEdited;
	ObjectsBeingEdited.Add(InSkillAsset);

	CreateDefaultCommands();

	// Initialize the asset editor and spawn tabs
	const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode,
		InitToolkitHost,
		SkillAssetEditorAppIdentifier,
		DummyLayout,
		bCreateDefaultStandaloneMenu,
		bCreateDefaultToolbar,
		ObjectsBeingEdited);
	
	
	TArray<UBlueprint*> Blueprints;
	Blueprints.Add(InSkillAsset);

	CommonInitialization(Blueprints, false);
	SKAEditorModeInuse=MakeShareable(new SkillAssetEditorAPPMode(SharedThis(this)));
	check(SKAEditorModeInuse.IsValid())
	
	AddApplicationMode(
		SkillAssetEditorAPPMode::SKAModeID,
		SKAEditorModeInuse.ToSharedRef());

	ExtendMenu();
	ExtendToolBar();
	RegenerateMenusAndToolbars();

	// Activate the initial mode (which will populate with a real layout)
	SetCurrentMode(SkillAssetEditorAPPMode::SKAModeID);

	// Post-layout initialization
	PostLayoutBlueprintEditorInitialization();


	UpdateAssetThumbnail();
	
	
	
	
	
}


FName FSkillAssetEditor::GetToolkitFName() const
{
	return ToolkitFName;
}

FText FSkillAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Skill Asset Editor");
}

FText FSkillAssetEditor::GetToolkitName() const
{
	return FText::FromName(GetToolkitFName());
}

FText FSkillAssetEditor::GetToolkitToolTipText() const
{
	return LOCTEXT("ToolTip", "Skill Asset Editor");
}

FString FSkillAssetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Skill Asset Editor").ToString();
}

FLinearColor FSkillAssetEditor::GetWorldCentricTabColorScale() const
{
	return FColor::Black;
}

bool FSkillAssetEditor::IsPrimaryEditor() const
{
	return true;
}





void FSkillAssetEditor::SetSkillAsset(USkillAsset* InSkillAsset)
{
	SkillAsset=InSkillAsset;
}


void FSkillAssetEditor::ExtendMenu()
{
	if (MenuExtender.IsValid())
	{
		RemoveMenuExtender(MenuExtender);
		MenuExtender.Reset();
	}
	
	MenuExtender = MakeShareable(new FExtender);
	AddMenuExtender(MenuExtender);
    // example of how to register an command
    FString WindowsubMenuName=this->GetToolMenuName().ToString();
	WindowsubMenuName+=".Window";
	UToolMenu* Menu=UToolMenus::Get()->ExtendMenu(FName(*WindowsubMenuName));
	{
		
		FToolMenuSection& Section=Menu->FindOrAddSection("General");
		Section.AddMenuEntryWithCommandList(FSkillEditorcommands::Get().Textfunc,SkillAssetExtcommands);
		Section.AddSubMenu("New Sub Menu", FText::FromString("???"), FText::FromString("???"),
					   FNewToolMenuChoice(
						   FNewMenuDelegate::CreateRaw(
							   this, &FSkillAssetEditor::FillsubMenu)));
	}
}

void FSkillAssetEditor::ExtendToolBar()
{
	// If the ToolbarExtender is valid, remove it before rebuilding it
	if (ToolbarExtender.IsValid())
	{
		RemoveToolbarExtender(ToolbarExtender);
		ToolbarExtender.Reset();
	}

	ToolbarExtender = MakeShareable(new FExtender);

	AddToolbarExtender(ToolbarExtender);
	FName ToolBarName;
	GetToolMenuToolbarName(ToolBarName);
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(ToolBarName);
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Asset");
		{
			FToolMenuEntry& Entry = Section.AddEntry(
				FToolMenuEntry::InitToolBarButton(FSkillEditorcommands::Get().Textfunc));
			Entry.SetCommandList(FSkillAssetEditor::SkillAssetExtcommands);
		}
	}
	
}

TSharedPtr<SKillAssetPriveiwScene> FSkillAssetEditor::GetPreviewSceneDirectly()
{
	{
		return SKAEditorModeInuse->GetThePreviewSceneInsideTheMode();
		
	}
}

void FSkillAssetEditor::UpdateAssetThumbnail()
{
	check(GetSkillAsset()!=nullptr);
	if(GetSkillAsset()->LastTimeAssetOwnerType==GetSkillAsset()->AssetOwnerType)
	{
		UE_LOG(LogTemp,Warning,L"No need to update the thumbnail")
	}
	else
	{
		FString ThumbNailName=IPluginManager::Get().FindPlugin("SkillEditor2D")->GetBaseDir() / TEXT("Resources");
	ThumbNailName+="/";
	switch (GetSkillAsset()->AssetOwnerType)
	{
	    case E_SkillAssetType::Umbrella:
		{
			ThumbNailName+="UmSword.png";
			break;
		}
	case E_SkillAssetType::B_Hammer:
		    {
	    		ThumbNailName+="Hammer.png";
			    break;
		    }
	case E_SkillAssetType::B_ClayMore:
		    {
	    		ThumbNailName+="ClayMore.png";
			    break;
		    }
	case E_SkillAssetType::S_DoubleBlade:
		    {
	    		ThumbNailName+="ClayMore.png";
			    break;
		    }
		default:
		    {
	    		ThumbNailName+="UmSword.png";
			    
		    }
	}
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	TArray<uint8> RawPNGImageData;
	TArray<uint8> RawBMPData;
	if(FFileHelper::LoadFileToArray(RawPNGImageData,*ThumbNailName))
	{
		if(ImageWrapper.IsValid()&&
			ImageWrapper->SetCompressed
			(RawPNGImageData.GetData(),
				RawPNGImageData.
				Num()))
		{
			if(ImageWrapper->GetRaw(ERGBFormat::BGRA,8,RawBMPData))
			{
				
			}
		}
	}
	uint32 SrcWidth = ImageWrapper->GetWidth();
	uint32 SrcHeight = ImageWrapper->GetHeight();
	TArray<FColor> OrigBitmap;
	OrigBitmap.Init(FColor(0,0,0,0),SrcHeight*SrcWidth);
	FMemory::Memcpy((void*)OrigBitmap.GetData(),(void*)RawBMPData.GetData(),SrcHeight*SrcWidth*4);
	
	check(OrigBitmap.Num() == SrcWidth * SrcHeight);
	//pin to smallest value
		int32 CropSize = FMath::Min<uint32>(SrcWidth, SrcHeight);
		//pin to max size
		int32 ScaledSize  = FMath::Min<uint32>(ThumbnailTools::DefaultThumbnailSize, CropSize);
 
		//calculations for cropping
		TArray<FColor> CroppedBitmap;
		CroppedBitmap.AddUninitialized(CropSize*CropSize);
		//Crop the image
		int32 CroppedSrcTop  = (SrcHeight - CropSize)/2;
		int32 CroppedSrcLeft = (SrcWidth - CropSize)/2;
		for (int32 Row = 0; Row < CropSize; ++Row)
		{
			//Row*Side of a row*byte per color
			int32 SrcPixelIndex = (CroppedSrcTop+Row)*SrcWidth + CroppedSrcLeft;
			const void* SrcPtr = &(OrigBitmap[SrcPixelIndex]);
			void* DstPtr = &(CroppedBitmap[Row*CropSize]);
			FMemory::Memcpy(DstPtr, SrcPtr, CropSize*4);
		}
 
		//Scale image down if needed
		TArray<FColor> ScaledBitmap;
		if (ScaledSize < CropSize)
		{
			FImageUtils::ImageResize( CropSize, CropSize, CroppedBitmap, ScaledSize, ScaledSize, ScaledBitmap, true );
		}
		else
		{
			//just copy the data over. sizes are the same
			ScaledBitmap = CroppedBitmap;
		}
 
		//setup actual thumbnail
		FObjectThumbnail TempThumbnail;
		TempThumbnail.SetImageSize( ScaledSize, ScaledSize );
		TArray<uint8>& ThumbnailByteArray = TempThumbnail.AccessImageData();
 
		// Copy scaled image into destination thumb
		int32 MemorySize = ScaledSize*ScaledSize*sizeof(FColor);
		ThumbnailByteArray.AddUninitialized(MemorySize);
		FMemory::Memcpy(&(ThumbnailByteArray[0]), &(ScaledBitmap[0]), MemorySize);
 
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	
		FAssetData CurrentUpdateAsset(GetSkillAsset());
		const FAssetData& CurrentAsset = CurrentUpdateAsset;
 
		//assign the thumbnail and dirty
		const FString ObjectFullName = CurrentAsset.GetFullName();
		const FString PackageName    = CurrentAsset.PackageName.ToString();
 
		UPackage* AssetPackage = FindObject<UPackage>( NULL, *PackageName );
		if ( ensure(AssetPackage) )
		{
			FObjectThumbnail* NewThumbnail = ThumbnailTools::CacheThumbnail(ObjectFullName, &TempThumbnail, AssetPackage);
			if ( ensure(NewThumbnail) )
			{
				//we need to indicate that the package needs to be resaved
				AssetPackage->MarkPackageDirty();
 
				// Let the content browser know that we've changed the thumbnail
				NewThumbnail->MarkAsDirty();
						
				// Signal that the asset was changed if it is loaded so thumbnail pools will update
				if ( CurrentAsset.IsAssetLoaded() )
				{
					CurrentAsset.GetAsset()->PostEditChange();
				}
 
				//Set that thumbnail as a valid custom thumbnail so it'll be saved out
				NewThumbnail->SetCreatedAfterCustomThumbsEnabled();
			}
		}
		GetSkillAsset()->LastTimeAssetOwnerType=GetSkillAsset()->AssetOwnerType;	
	}
	

	
}

void FSkillAssetEditor::FillToolbar(FToolBarBuilder& ToolBarbuilder)
{
	ToolBarbuilder.BeginSection("ExtendToolbarItem");
	{
		const FText NewNodeLabel = LOCTEXT("NewNode_Label", "New");
		const FText NewNodeTooltip = LOCTEXT("NewNode_ToolTip", "Create a new node");
		const FSlateIcon NewNodeIcon = FSlateIcon(TEXT("EditorStyle"), "SessionConsole.Clear");

		ToolBarbuilder.AddToolBarButton(
			FUIAction(
				FExecuteAction::CreateSP(this, &FSkillAssetEditor::TextFuncOncliked),
				FCanExecuteAction::CreateSP(this, &FSkillAssetEditor::canExecuteBar),
				FIsActionChecked()
			),
			NAME_None,
			NewNodeLabel,
			NewNodeTooltip,
			NewNodeIcon
		);
	}
	ToolBarbuilder.EndSection();
}
void FSkillAssetEditor::FillsubMenu(FMenuBuilder& Menubuilder)
{
	{
		// Create the Submenu Entries
 
		Menubuilder.AddMenuEntry(
			FSkillEditorcommands::Get().Textfunc, NAME_None,
			FText::FromString("Menu Entry 1"),
			FText::FromString("Menu Entry 1 Tooltip"),
			FSlateIcon()
		);
	}
}
void FSkillAssetEditor::TextFuncOncliked()
{
	UE_LOG(LogTemp,Warning,L"Textfunconcliced!")
}



USkillAsset* FSkillAssetEditor::GetSkillAsset()
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	for (int32 i = 0; i < EditingObjs.Num(); ++i)
	{
		if (EditingObjs[i]->IsA<USkillAsset>()) { return (USkillAsset*)EditingObjs[i]; }
	}
	return nullptr;
}


#undef LOCTEXT_NAMESPACE
