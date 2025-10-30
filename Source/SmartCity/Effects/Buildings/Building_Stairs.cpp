#include "Building_Stairs.h"

#include "SceneInteractionDecorator.h"
#include "SceneInteractionWorldSystem.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"

void ABuilding_Stairs::SwitchInteractionType(
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

		//  只要是楼层就显示
		if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer))
			// if (ConditionalSet.ConditionalSet.HasAll(EmptyContainer) && ConditionalSet.ConditionalSet.Num() ==
				//     EmptyContainer.Num())
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
				const auto ViewConfig = DecoratorSPtr->GetViewConfig();
				if (ViewConfig.StairsTranlucent <= 0)
				{
					SwitchState(EState::kHiden);
				}
				else if (ViewConfig.StairsTranlucent >= 100)
				{
					SwitchState(EState::kOriginal);
				}
				else
				{
					SetTranslucent(ViewConfig.StairsTranlucent);
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
