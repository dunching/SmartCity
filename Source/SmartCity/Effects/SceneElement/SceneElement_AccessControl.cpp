#include "SceneElement_AccessControl.h"

#include "ActorSequenceComponent.h"

#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "SmartCitySuiteTags.h"

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

void ASceneElement_AccessControl::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	 Super::SwitchInteractionType(ConditionalSet);
	
	{
	 	if (
			 ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall) ||
			 ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		 )
		{
			SetActorHiddenInGame(true);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
			EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}
		SetActorHiddenInGame(true);

		return;
	}
}
