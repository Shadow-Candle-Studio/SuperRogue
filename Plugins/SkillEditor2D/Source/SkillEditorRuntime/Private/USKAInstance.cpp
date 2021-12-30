// Fill out your copyright notice in the Description page of Project Settings.


#include "USKAInstance.h"

#include "SkillAsset.h"

USKAInstance::USKAInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
	UE_LOG(LogTemp,Warning,L"USK des %d",((USkillAsset*)GetOutermostObject())->num)
}
void USKAInstance::PostInitProperties()
{
	UObject::PostInitProperties();
}
