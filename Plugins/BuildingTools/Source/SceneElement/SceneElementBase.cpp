#include "SceneElementBase.h"

ASceneElementBase::ASceneElementBase(
	const FObjectInitializer& ObjectInitializer
	) :
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
		AActor* ParentPtr = ActorRef.LoadSynchronous()->GetAttachParentActor();
		if (ParentPtr && !GetAttachParentActor())
		{
			AttachToActor(ParentPtr, FAttachmentTransformRules::KeepWorldTransform);
			SetActorTransform(ActorRef.LoadSynchronous()->GetTransform());
		}

		TArray<AActor*> OutActors;
		ActorRef.LoadSynchronous()->GetAttachedActors(OutActors);
		for (auto Iter : OutActors)
		{
			Iter->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
		ReplaceImp(ActorRef.LoadSynchronous());

#if WITH_EDITOR
		SceneElementName = ActorRef->GetActorLabel();

		SetActorLabel(SceneElementName);
#endif // WITH_EDITOR

		ActorRef->Destroy();
	}
}

void ASceneElementBase::ReplaceImp(
	AActor* ActorPtr
	)
{
	if (ActorPtr)
	{
	}
}

void ASceneElementBase::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& UserData
	)
{
	if (ActorRef.ToSoftObjectPath().IsValid())
	{
		AActor* ParentPtr = ActorRef.LoadSynchronous()->GetAttachParentActor();
		if (ParentPtr && !GetAttachParentActor())
		{
			AttachToActor(ParentPtr, FAttachmentTransformRules::KeepWorldTransform);
			SetActorTransform(ActorRef.LoadSynchronous()->GetTransform());
		}

		TArray<AActor*> OutActors;
		ActorRef.LoadSynchronous()->GetAttachedActors(OutActors);
		for (auto Iter : OutActors)
		{
			Iter->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
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
