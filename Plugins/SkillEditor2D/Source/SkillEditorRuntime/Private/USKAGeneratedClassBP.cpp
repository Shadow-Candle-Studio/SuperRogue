// Fill out your copyright notice in the Description page of Project Settings.


#include "USKAGeneratedClassBP.h"

#include "Engine/SCS_Node.h"

USKAGeneratedClassBP::USKAGeneratedClassBP(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer){}

EDataValidationResult USKAGeneratedClassBP::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = EDataValidationResult::NotValidated;
	if (SimpleConstructionScript)
	{
		for (const USCS_Node* Node : SimpleConstructionScript->GetAllNodes())
		{
			if (Node)
			{
				UActorComponent* ActorComponent = Node->GetActualComponentTemplate(this);
				if (ActorComponent)
				{
					const EDataValidationResult ValidationResult = ActorComponent->IsDataValid(ValidationErrors);
					// Only allow upward escalation from NotValidated -> Valid -> Invalid
					if (static_cast<uint8>(ValidationResult) < static_cast<uint8>(Result))
					{
						Result = ValidationResult;
					}
				}
			}
		}
         
	}
	const EDataValidationResult DefaultObjectResult = GetDefaultObject()->IsDataValid(ValidationErrors);
	// Only allow upward escalation from NotValidated -> Valid -> Invalid
	if (static_cast<uint8>(DefaultObjectResult) < static_cast<uint8>(Result))
	{
		Result = DefaultObjectResult;
	}
	return Result;
}

void USKAGeneratedClassBP::Link(FArchive& Ar, bool bRelinkExistingProperties)
{
	Super::Link(Ar, bRelinkExistingProperties);
	
	USKAGeneratedClassBP* RootClass = this;
	while (USKAGeneratedClassBP* NextClass = Cast<USKAGeneratedClassBP>(RootClass->GetSuperClass()))
	{
		RootClass = NextClass;
	}
}

void USKAGeneratedClassBP::PurgeClass(bool bRecompilingOnLoad)
{
	Super::PurgeClass(bRecompilingOnLoad);
}

void USKAGeneratedClassBP::PostLoadDefaultObject(UObject* Object)
{
	Super::PostLoadDefaultObject(Object);
	//CastChecked<UPixel2DAnimInstance>(Object)->PreInitializeRootNode();
}
