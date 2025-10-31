#include "SceneElementBase.h"

ASceneElementBase::ASceneElementBase(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void ASceneElementBase::InitialSceneElement()
{
}

void ASceneElementBase::Replace(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData
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
		ReplaceImp(ActorRef.LoadSynchronous(), InUserData);

#if WITH_EDITOR
		SceneElementName = ActorRef->GetActorLabel();

		SetActorLabel(SceneElementName);
#endif // WITH_EDITOR

		ActorRef->Destroy();
	}
}

void ASceneElementBase::ReplaceImp(
	AActor* ActorPtr,
	const TPair<FName, FString>& InUserData
	)
{
	if (ActorPtr)
	{
	}
}

void ASceneElementBase::Merge(
	const TSoftObjectPtr<AActor>& ActorRef,
	const TPair<FName, FString>& InUserData
	)
{
	if (ActorRef.ToSoftObjectPath().IsValid())
	{
		AActor* ParentPtr = ActorRef.LoadSynchronous()->GetAttachParentActor();
		if (ParentPtr && !GetAttachParentActor())
		{
			AttachToActor(ParentPtr, FAttachmentTransformRules::KeepWorldTransform);
			// SetActorTransform(ActorRef.LoadSynchronous()->GetTransform());
			SetActorTransform(FTransform::Identity);
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

void ASceneElementBase::QuitAllState()
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

TSharedPtr<FJsonValue> ASceneElementBase::GetSceneElementData() const
{
	auto RootJsonObj = MakeShared<FJsonObject>();

	auto Result = MakeShared<FJsonValueObject>(RootJsonObj);

	RootJsonObj->SetStringField(
	                            TEXT("ID"),
	                            SceneElementID
	                           );

	RootJsonObj->SetStringField(
	                            TEXT("CurrentConditionalSet"),
	                            CurrentConditionalSet.ConditionalSet.ToString()
	                           );

	return Result;
}

void ASceneElementBase::RevertOnriginalMat()
{
	TArray<UStaticMeshComponent*> Components;
	GetComponents<UStaticMeshComponent>(Components);

	for (auto Iter : Components)
	{
		if (!Iter)
		{
			continue;
		}
		auto MatAry = OriginalMaterials.Find(Iter);
		if (!MatAry)
		{
			continue;
		}

		const auto MatNum = Iter->GetMaterials().Num();
		const auto OriMatNum = MatAry->MaterialsCacheAry.Num();
		for (int32 Index = 0; Index < MatNum && Index < OriMatNum; Index++)
		{
			Iter->SetMaterial(Index, MatAry->MaterialsCacheAry[Index]);
		}
	}

	OriginalMaterials.Empty();
}

void ASceneElementBase::SetTranslucentImp(
	const TArray<UStaticMeshComponent*>& Components,
	int32 Value,
	const TSoftObjectPtr<UMaterialInstance>& MatRef
	)
{
	CacheOriginalMat(Components);

	auto WallTranslucentMatInst = MatRef.LoadSynchronous();
	auto MatInstDynamicPtr = UMaterialInstanceDynamic::Create(WallTranslucentMatInst, this);

	MatInstDynamicPtr->SetScalarParameterValue(TEXT("Translucent"), Value / 100.f);

	for (auto Iter : Components)
	{
		if (Iter)
		{
			const auto MatNum = Iter->GetMaterials().Num();
			for (int32 Index = 0; Index < MatNum; Index++)
			{
				Iter->SetMaterial(Index, MatInstDynamicPtr);
			}
		}
	}
}

void ASceneElementBase::CacheOriginalMat(
	const TArray<UStaticMeshComponent*>& Components
	)
{
	for (auto Iter : Components)
	{
		if (!Iter)
		{
			continue;
		}
		if (OriginalMaterials.Contains(Iter))
		{
			continue;
		}

		FMaterialsCache MaterialAry;
		auto Mats = Iter->GetMaterials();
		for (auto MatIter : Mats)
		{
			MaterialAry.MaterialsCacheAry.Add(MatIter);
		}

		OriginalMaterials.Add(Iter, MaterialAry);
	}
}
