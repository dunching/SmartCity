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
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
			)
		{
			SwitchState(EState::kOriginal);

			return;
		}
	}
	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Floor)||
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting)
			)
		{
			SwitchState(EState::kOriginal);

			return;
		}
	}
	{
		//  只要是楼层就显示
		if (
			ConditionalSet.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor)
		)
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
				if (ViewConfig.WallTranlucent <= 0)
				{
					SwitchState(EState::kHiden);
				}
				else if (ViewConfig.WallTranlucent >= 100)
				{
					SwitchState(EState::kOriginal);
				}
				else
				{
					SetTranslucent(ViewConfig.WallTranlucent);
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
