#include "SceneElement_DeviceBase.h"

#include "ActorSequenceComponent.h"

#include "CollisionDataStruct.h"

void ASceneElement_DeviceBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASceneElement_DeviceBase::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);

	TArray<UStaticMeshComponent*> Components; 
	GetComponents<UStaticMeshComponent>(Components);
	for (auto Iter : Components)
	{
		if (Iter)
		{
			Iter->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Iter->SetCollisionObjectType(Device_Object);
			Iter->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

			Iter->SetRenderCustomDepth(false);

			break;
		}
	}
}
