#include "SceneElementBase.h"

#include "ActorSequenceComponent.h"

#include "CollisionDataStruct.h"

ASceneElementBase::ASceneElementBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void ASceneElementBase::Replace(
	const TSoftObjectPtr<AActor>& ActorRef
	)
{
	if (ActorRef.ToSoftObjectPath().IsValid())
	{
		AActor* ParentPtr = ActorRef->GetAttachParentActor();
		if (ParentPtr)
		{
			AttachToActor(ParentPtr, FAttachmentTransformRules::KeepRelativeTransform);
		}

		ActorRef->Destroy();
	}
}
