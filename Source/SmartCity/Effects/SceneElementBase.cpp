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

		ReplaceImp(ActorRef.LoadSynchronous());

		SceneElementName = ActorRef->GetActorLabel();
		// SceneElementName = ActorRef->GetName();
		
		ActorRef->Destroy();
	}
}

void ASceneElementBase::ReplaceImp(
	AActor* ActorPtr
	)
{
}

void ASceneElementBase::BeginInteraction()
{
}

void ASceneElementBase::EndInteraction()
{
}

TMap<FString, FString> ASceneElementBase::GetStateDescription() const
{
	return {};
}

void ASceneElementBase::SwitchInteractionType(
	EInteractionType InInteractionType
	)
{
	InteractionType = InInteractionType;
}
