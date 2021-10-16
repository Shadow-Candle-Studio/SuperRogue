// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Interfaces/Interface_AssetUserData.h"
#include "Pixel2DAnimAsset.generated.h"

class UAssetUserData;


UCLASS(abstract, MinimalAPI)
class UPixel2DAnimAsset : public UObject, public IInterface_AssetUserData
{
	GENERATED_UCLASS_BODY()

public:

#if WITH_EDITORONLY_DATA
	/** Parent Asset, if set, you won't be able to edit any data in here but just mapping table
	*
	* During cooking, this data will be used to bake out to normal asset */
	UPROPERTY(AssetRegistrySearchable, Category = Animation, VisibleAnywhere)
	class UPixel2DAnimAsset* ParentAsset;

	/**
	* note this is transient as they're added as they're loaded
	*/
	UPROPERTY(transient)
	TArray<class UPixel2DAnimAsset*> ChildrenAssets;

#endif // WITH_EDITORONLY_DATA

protected:
	/** Array of user data stored with the asset */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Instanced, Category = Animation)
	TArray<UAssetUserData*> AssetUserData;

public:
	/** Return a list of unique marker names for blending compatibility */
	PIXEL2D_API virtual TArray<FName>* GetUniqueMarkerNames() { return NULL; }

	//~ Begin IInterface_AssetUserData Interface
	PIXEL2D_API virtual void AddAssetUserData(UAssetUserData* InUserData) override;
	PIXEL2D_API virtual void RemoveUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass) override;
	PIXEL2D_API virtual UAssetUserData* GetAssetUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass) override;
	PIXEL2D_API virtual const TArray<UAssetUserData*>* GetAssetUserDataArray() const override;
	//~ End IInterface_AssetUserData Interface

};

