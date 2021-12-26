// Fill out your copyright notice in the Description page of Project Settings.


#include "USKAInstance.h"
USKAInstance::USKAInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogTemp,Warning,L"USK Instance crearted!")
}
void USKAInstance::PostInitProperties()
{
	UObject::PostInitProperties();
}
