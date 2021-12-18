// Fill out your copyright notice in the Description page of Project Settings.


#include "SKillAssetPriveiwScene.h"

#include "Camera/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "GameFramework/WorldSettings.h"

// control all assets rendered in the scene
SKillAssetPriveiwScene::SKillAssetPriveiwScene(ConstructionValues CVS):FAdvancedPreviewScene(CVS)
{
	//world setting initialization
	GetWorld()->GetWorldSettings()->NotifyBeginPlay();
	GetWorld()->GetWorldSettings()->NotifyMatchStarted();
	GetWorld()->GetWorldSettings()->SetActorHiddenInGame(false);
	GetWorld()->bBegunPlay=true;
	
	//set light option
	DirectionalLight->SetRelativeLocation
	(FVector(-1024.f,1024.f,2048.f));
	DirectionalLight->SetRelativeScale3D(FVector(15.f));
	SetLightBrightness(4.f);
	DirectionalLight->InvalidateLightingCache();
	DirectionalLight->RecreateRenderState_Concurrent();
	
	
	SkyLight->SetRelativeLocation(FVector(-102.f,102.f,204.f));
	SkyLight->SetIntensity(20.f);
	//create a sky sphere
	UStaticMeshComponent* SkyComp=NewObject<UStaticMeshComponent>();
	UStaticMesh* StaticMesh=LoadObject<UStaticMesh>(NULL,
		TEXT("/Engine/MapTemplates/Sky/SM_SkySphere.SM_SkySphere"),NULL,LOAD_None,NULL);
	SkyComp->SetStaticMesh(StaticMesh);
	UMaterial* SkyMaterial=LoadObject
	<UMaterial>
	(NULL, TEXT
		("/Engine/EditorMaterials/PersonaSky.PersonaSky"),
		NULL,
		LOAD_None,
		NULL);
	SkyComp->SetMaterial(0,SkyMaterial);
	const float SkySphereScale=1000.f;
	const FTransform SkytransForm
	(FRotator(0,0,0),FVector(0,0,0),
		FVector(SkySphereScale));
	AddComponent(SkyComp,SkytransForm);
	
	//add floor
	UStaticMesh* FloorMesh = LoadObject<UStaticMesh>(NULL,
		TEXT("/Engine/EditorMeshes/EditorCube.EditorCube"),
		NULL, LOAD_None,
		NULL);
	UStaticMeshComponent* FloorComp = NewObject<UStaticMeshComponent>();
	FloorComp->SetStaticMesh(FloorMesh);
	AddComponent(FloorComp, FTransform::Identity);
	FloorComp->SetRelativeScale3D(FVector(30.f, 30.f, 10.f));
	UMaterial* Material = LoadObject<UMaterial>(NULL, TEXT("/Engine/EditorMaterials/PersonaFloorMat.PersonaFloorMat"), NULL, LOAD_None, NULL);
	FloorComp->SetMaterial(0, Material);
	FloorComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FloorComp->SetCollisionObjectType(ECC_WorldStatic);


	
}

SKillAssetPriveiwScene::~SKillAssetPriveiwScene()
{
}
