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
		AActor* ParentPtr = ActorPtr->GetAttachParentActor();
		if (ParentPtr && !GetAttachParentActor())
		{
			AttachToActor(ParentPtr, FAttachmentTransformRules::KeepWorldTransform);
		}
		SetActorTransform(ActorPtr->GetTransform());
		
		TArray<AActor*> OutActors;
		ActorPtr->GetAttachedActors(OutActors);
		for (auto Iter : OutActors)
		{
			Iter->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	
	}
}

void ASceneElementBase::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& UserData
	)
{
}

void ASceneElementBase::MergeWithNear(
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
