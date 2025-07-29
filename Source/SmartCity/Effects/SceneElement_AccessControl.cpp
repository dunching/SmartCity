#include "SceneElement_AccessControl.h"

#include "ActorSequenceComponent.h"

#include "CollisionDataStruct.h"

ASceneElement_AccessControl::ASceneElement_AccessControl(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	ChestMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMeshComponent"));
	ChestMeshComponent->SetupAttachment(RootComponent);

	ChestMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ChestMeshComponent->SetCollisionObjectType(Device_Object);
	ChestMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	FanAncherMeshComponent = CreateDefaultSubobject<USceneComponent>(TEXT("FanAncherMeshComponent"));
	FanAncherMeshComponent->SetupAttachment(RootComponent);

	FanMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FanMeshComponent"));
	FanMeshComponent->SetupAttachment(FanAncherMeshComponent);

	FanMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FanMeshComponent->SetCollisionObjectType(Device_Object);
	FanMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}
