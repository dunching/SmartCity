#include "SceneElementBase.h"

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
			AttachToActor(ParentPtr, FAttachmentTransformRules::KeepWorldTransform);
		}

		ReplaceImp(ActorRef.LoadSynchronous());

		SceneElementName = ActorRef->GetActorLabel();
		
		SetActorLabel(SceneElementName);
		
		ActorRef->Destroy();
	}
}

void ASceneElementBase::ReplaceImp(
	AActor* ActorPtr
	)
{
}

void ASceneElementBase::Merge(
	const TSoftObjectPtr<AActor>& ActorRef
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
	const FSceneElementConditional& ConditionalSet
	)
{
	CurrentConditionalSet = ConditionalSet;
}
