// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UnrealType.h"
#include "Editor.h"
#include "K2Node.h"
#include "Pixel2DAnimBlueprint.h"
#include "AnimGraphNode_Base.h"
#include "Pixel2DAnimGraphNode_Base.generated.h"

class FPixel2DAnimBlueprintCompilerContext;
class UPixel2DAnimBlueprintGeneratedClass;
class FAnimGraphNodeDetails;
class FBlueprintActionDatabaseRegistrar;
class FCompilerResultsLog;
class IDetailLayoutBuilder;
class UEdGraphSchema;
class UPixel2DAnimGraphNode_Base;

struct FPixel2DPoseLinkMappingRecord
{
public:
	static FPixel2DPoseLinkMappingRecord MakeFromArrayEntry(UPixel2DAnimGraphNode_Base* LinkingNode, UPixel2DAnimGraphNode_Base* LinkedNode, FArrayProperty* ArrayProperty, int32 ArrayIndex)
	{
		checkSlow(CastChecked<FStructProperty>(ArrayProperty->Inner)->Struct->IsChildOf(FPixel2DPoseLinkBase::StaticStruct()));

		FPixel2DPoseLinkMappingRecord Result;
		Result.LinkingNode = LinkingNode;
		Result.LinkedNode = LinkedNode;
		Result.ChildProperty = ArrayProperty;
		Result.ChildPropertyIndex = ArrayIndex;

		return Result;
	}

	static FPixel2DPoseLinkMappingRecord MakeFromMember(UPixel2DAnimGraphNode_Base* LinkingNode, UPixel2DAnimGraphNode_Base* LinkedNode, FStructProperty* MemberProperty)
	{
		checkSlow(MemberProperty->Struct->IsChildOf(FPixel2DPoseLinkBase::StaticStruct()));

		FPixel2DPoseLinkMappingRecord Result;
		Result.LinkingNode = LinkingNode;
		Result.LinkedNode = LinkedNode;
		Result.ChildProperty = MemberProperty;
		Result.ChildPropertyIndex = INDEX_NONE;

		return Result;
	}

	static FPixel2DPoseLinkMappingRecord MakeInvalid()
	{
		FPixel2DPoseLinkMappingRecord Result;
		return Result;
	}

	bool IsValid() const
	{
		return LinkedNode != nullptr;
	}

	UPixel2DAnimGraphNode_Base* GetLinkedNode() const
	{
		return LinkedNode;
	}

	UPixel2DAnimGraphNode_Base* GetLinkingNode() const
	{
		return LinkingNode;
	}

	PIXEL2DEDITOR_API void PatchLinkIndex(uint8* DestinationPtr, int32 LinkID, int32 SourceLinkID) const;
protected:
	FPixel2DPoseLinkMappingRecord()
		: LinkedNode(nullptr)
		, LinkingNode(nullptr)
		, ChildProperty(nullptr)
		, ChildPropertyIndex(INDEX_NONE)
	{
	}

protected:
	// Linked node for this pose link, can be nullptr
	UPixel2DAnimGraphNode_Base* LinkedNode;

	// Linking node for this pose link, can be nullptr
	UPixel2DAnimGraphNode_Base* LinkingNode;

	// Will either be an array property containing FPoseLinkBase derived structs, indexed by ChildPropertyIndex, or a FPoseLinkBase derived struct property 
	FProperty* ChildProperty;

	// Index when ChildProperty is an array
	int32 ChildPropertyIndex;
};

/**
* This is the base class for any animation graph nodes that generate or consume an animation pose in
* the animation blend graph.
*
* Any concrete implementations will be paired with a runtime graph node derived from FAnimNode_Base
*/
UCLASS(Abstract)
class PIXEL2DEDITOR_API UPixel2DAnimGraphNode_Base : public UK2Node
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = PinOptions, EditFixedSize)
	TArray<FOptionalPinFromProperty> ShowPinForProperties;

	UPROPERTY(Transient)
	EBlueprintUsage BlueprintUsage;

	// UObject interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	// End of UObject interface

	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	virtual bool ShowPaletteIconOnNode() const override { return false; }
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual bool NodeCausesStructuralBlueprintChange() const override { return true; }
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual bool CanPlaceBreakpoints() const override { return false; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* DesiredSchema) const override;
	virtual void GetNodeAttributes(TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;

	// By default return any animation assets we have
	virtual UObject* GetJumpTargetForDoubleClick() const override { return nullptr; }
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	// End of UK2Node interface

	// UAnimGraphNode_Base interface

	// Gets the menu category this node belongs in
	virtual FString GetNodeCategory() const;

	// Is this node a sink that has no pose outputs?
	virtual bool IsSinkNode() const { return false; }

	// Create any output pins necessary for this node
	virtual void CreateOutputPins();

	// customize pin data based on the input
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const {}

	// Gives each visual node a chance to do final validation before it's node is harvested for use at runtime
	virtual void ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog) {}

	// Gives each visual node a chance to validate that they are still valid in the context of the compiled class, giving a last shot at error or warning generation after primary compilation is finished
	virtual void ValidateAnimNodePostCompile(FCompilerResultsLog& MessageLog, UPixel2DAnimBlueprintGeneratedClass* CompiledClass, int32 CompiledNodeIndex) {}

	// Gives each visual node a chance to update the node template before it is inserted in the compiled class
	virtual void BakeDataDuringCompilation(FCompilerResultsLog& MessageLog) {}

	// Give the node a chance to change the display name of a pin
	virtual void PostProcessPinName(const UEdGraphPin* Pin, FString& DisplayName) const;

	/** Get the animation blueprint to which this node belongs */
	UPixel2DAnimBlueprint* GetAnimBlueprint() const { return CastChecked<UPixel2DAnimBlueprint>(GetBlueprint()); }

	/**
	* Override this function to push an editor mode when this node is selected
	* @return the editor mode to use when this node is selected
	*/
	virtual FEditorModeID GetEditorMode() const;

	/** Called after editing a default value to update internal node from pin defaults. This is needed for forwarding code to propagate values to preview. */
	virtual void CopyPinDefaultsToNodeData(UEdGraphPin* InPin) {}

	// can customize details tab 
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) { }

	template<typename NodeType>
	NodeType* GetActiveInstanceNode(UObject* AnimInstanceObject) const
	{
		if (!AnimInstanceObject)
		{
			return nullptr;
		}

		if (UAnimBlueprintGeneratedClass* AnimClass = Cast<UAnimBlueprintGeneratedClass>(AnimInstanceObject->GetClass()))
		{
			return AnimClass->GetPropertyInstance<NodeType>(AnimInstanceObject, NodeGuid);
		}

		return nullptr;
	}

	/**
	*	Returns whether this node supports the supplied asset class
	*	@param	bPrimaryAssetHandler	Is this the 'primary' handler for this asset (the node that should be created when asset is dropped)
	*/
	virtual EAnimAssetHandlerType SupportsAssetClass(const UClass* AssetClass) const;

	// Event that observers can bind to so that they are notified about changes
	// made to this node through the property system
	DECLARE_EVENT_OneParam(UPixel2DAnimGraphNode_Base, FOnNodePropertyChangedEvent, FPropertyChangedEvent&);
	FOnNodePropertyChangedEvent& OnNodePropertyChanged() { return PropertyChangeEvent; }

	/**
	* Helper function to check whether a pin is valid and linked to something else in the graph
	* @param	InPinName		The name of the pin @see UEdGraphNode::FindPin
	* @param	InPinDirection	The direction of the pin we are looking for. If this is EGPD_MAX, all directions are considered
	* @return true if the pin is present and connected
	*/
	bool IsPinExposedAndLinked(const FString& InPinName, const EEdGraphPinDirection Direction = EGPD_MAX) const;

protected:
	friend FPixel2DAnimBlueprintCompilerContext;
	friend FAnimGraphNodeDetails;

	// Gets the animation FNode type represented by this ed graph node
	UScriptStruct* GetFNodeType() const;

	// Gets the animation FNode property represented by this ed graph node
	FStructProperty* GetFNodeProperty() const;

	//
	virtual FPixel2DPoseLinkMappingRecord GetLinkIDLocation(const UScriptStruct* NodeType, UEdGraphPin* InputLinkPin);

	/** Get the property (and possibly array index) associated with the supplied pin */
	virtual void GetPinAssociatedProperty(const UScriptStruct* NodeType, const UEdGraphPin* InputPin, FProperty*& OutProperty, int32& OutIndex) const;

	// Allocates or reallocates pins
	void InternalPinCreation(TArray<UEdGraphPin*>* OldPins);

	FOnNodePropertyChangedEvent PropertyChangeEvent;

private:
	TArray<FName> OldShownPins;
};