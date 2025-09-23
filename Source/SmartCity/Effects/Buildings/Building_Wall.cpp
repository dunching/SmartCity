#include "Building_Wall.h"

#include "SceneInteractionDecorator.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"

void ABuilding_Wall::SwitchInteractionType(
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
			SwitchState(EState::kOriginal);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		EmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor.GetTag());

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
			EmptyContainer.Num())
		{
			// 确认当前的模式
			auto DecoratorSPtr =
				DynamicCastSharedPtr<FInteraction_Decorator>(
															 USceneInteractionWorldSystem::GetInstance()->
															 GetDecorator(
																		  USmartCitySuiteTags::Interaction_Interaction
																		 )
															);
			if (DecoratorSPtr)
			{
				if (DecoratorSPtr->Config.WallTranlucent <= 0)
				{
					SwitchState(EState::kHiden);
				}
				else if (DecoratorSPtr->Config.WallTranlucent >= 100)
				{
					SwitchState(EState::kOriginal);
				}
				else
				{
					SetTranslucent(DecoratorSPtr->Config.WallTranlucent);
				}

				return;
			}

			SwitchState(EState::kOriginal);

			return;
		}
	}
	{
		auto EmptyContainer = FGameplayTagContainer::EmptyContainer;

		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
			EmptyContainer.Num())
		{
			SwitchState(EState::kHiden);

			return;
		}
	}
}
