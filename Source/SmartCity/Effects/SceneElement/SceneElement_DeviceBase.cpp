#include "SceneElement_DeviceBase.h"

#include "ActorSequenceComponent.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"

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

void ASceneElement_DeviceBase::BeginInteraction()
{
	Super::BeginInteraction();

	bIsOpened = true;
}

void ASceneElement_DeviceBase::EndInteraction()
{
	Super::EndInteraction();

	bIsOpened = false;
}

TMap<FString, FString> ASceneElement_DeviceBase::GetStateDescription() const
{
	TMap<FString, FString> Result;

	if (bIsOpened)
	{
		Result.Add(TEXT(""), TEXT("开启"));
	}
	else
	{
		Result.Add(TEXT(""), TEXT("关闭"));
	}

	return Result;
}

void ASceneElement_DeviceBase::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
			SetActorHiddenInGame(true);
		
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_ExternalWall);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(true);
		
			auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(false);
			}
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Mode_ELV_Radar);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

			auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(true);
				PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
			}
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer ;
	
		EmptyContainer.AddTag(UGameplayTagsLibrary::Interaction_Area_Floor);
	
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
		{
			SetActorHiddenInGame(false);

			auto PrimitiveComponentPtr = GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(false);
			}
			
			return;
		}
	}
}
