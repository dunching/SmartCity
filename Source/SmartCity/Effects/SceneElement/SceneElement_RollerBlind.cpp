#include "SceneElement_RollerBlind.h"

#include "ActorSequenceComponent.h"
#include "ActorSequencePlayer.h"

#include "AssetRefMap.h"
#include "SmartCitySuiteTags.h"
#include "Kismet/KismetMathLibrary.h"

ASceneElement_RollerBlind::ASceneElement_RollerBlind(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
}

void ASceneElement_RollerBlind::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	// Super::SwitchInteractionType(ConditionalSet);

	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_ExternalWall);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			PlayAnimation(0);
			
			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);
		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			PlayAnimation(UKismetMathLibrary::RandomFloatInRange(0,1));
			
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
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
		    EmptyContainer.Num())
		{
			SetActorHiddenInGame(false);

			PlayAnimation(0);
			
			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}

		//
		SetActorHiddenInGame(true);

		return;
	}
}

void ASceneElement_RollerBlind::PlayAnimation(
	float Percent
	)
{
	if (MySequenceComponent->GetSequencePlayer() != nullptr)
	{
		const auto Second = MySequenceComponent->GetSequencePlayer()->GetDuration().AsSeconds();
		MySequenceComponent->GetSequencePlayer()->SetTimeRange(0, Percent * Duration);
		MySequenceComponent->GetSequencePlayer()->Play();
	}
}
