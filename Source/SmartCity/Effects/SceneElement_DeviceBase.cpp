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

			Iter->SetCollisionResponseToChannel(ExternalWall_Object, ECollisionResponse::ECR_Overlap);
			Iter->SetCollisionResponseToChannel(Floor_Object, ECollisionResponse::ECR_Overlap);
			Iter->SetCollisionResponseToChannel(Space_Object, ECollisionResponse::ECR_Overlap);

			Iter->SetRenderCustomDepth(false);

			break;
		}
	}
}

void ASceneElement_DeviceBase::SwitchFocusState(
	bool bIsFocus
	)
{
	Super::SwitchFocusState(bIsFocus);

	if (bIsFocus)
	{
		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetRenderCustomDepth(true);
			PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
		}
	}
	else
	{
		auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponentPtr)
		{
			PrimitiveComponentPtr->SetRenderCustomDepth(false);
		}
	}
}
