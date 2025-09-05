#include "SceneElement_InfraredDetector.h"

#include "Components/BoxComponent.h"
#include "Marks/PersonMark.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "FloorHelper.h"
#include "GameplayTagsLibrary.h"
#include "SmartCitySuiteTags.h"
#include "PersonMark.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionWorldSystem.h"
#include "TemplateHelper.h"

ASceneElement_InfraredDetector::ASceneElement_InfraredDetector(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	AnchorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AnchorComponent"));
	AnchorComponent->SetupAttachment(StaticMeshComponent);

	SweepEffectStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(
		 TEXT("SweepEffectStaticMeshComponent")
		);
	SweepEffectStaticMeshComponent->SetupAttachment(AnchorComponent);
}

void ASceneElement_InfraredDetector::OnConstruction(
	const FTransform& Transform
	)
{
	Super::OnConstruction(Transform);
}

void ASceneElement_InfraredDetector::ReplaceImp(
	AActor* ActorPtr
	)
{
	Super::ReplaceImp(ActorPtr);
	
	if (ActorPtr && ActorPtr->IsA(AStaticMeshActor::StaticClass()))
	{
		auto STPtr = Cast<AStaticMeshActor>(ActorPtr);
		if (STPtr)
		{
			StaticMeshComponent->SetStaticMesh(STPtr->GetStaticMeshComponent()->GetStaticMesh());
		}
	}
}

void ASceneElement_InfraredDetector::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(true);
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SweepEffectStaticMeshComponent->SetHiddenInGame(false);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SweepEffectStaticMeshComponent->SetHiddenInGame(false);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Interaction.GetTag());
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			SweepEffectStaticMeshComponent->SetHiddenInGame(false);

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

			SweepEffectStaticMeshComponent->SetHiddenInGame(true);
			
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
