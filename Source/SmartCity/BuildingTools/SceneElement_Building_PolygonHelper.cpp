#include "SceneElement_Building_PolygonHelper.h"

#include "SceneInteractionDecorator_Option.h"
#include "SceneInteractionWorldSystem.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"

void ASceneElement_Building_PolygonHelper::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_ExternalWall)
		)
		{
			EntryShowEffect(ConditionalSet);

			return;
		}
	}
	{
		if (ConditionalSet.ConditionalSet.IsEmpty())
		{
		}

		QuitAllState();

		return;
	}
}

void ASceneElement_Building_PolygonHelper::EntryFocusDevice(
	const FSceneElementConditional& ConditionalSet
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
		switch (DecoratorSPtr->GetInteractionType())
		{
		case EInteractionType::kDevice:
			{
				SetActorHiddenInGame(true);
			}
			break;
		case EInteractionType::kSpace:
			{
				SetActorHiddenInGame(false);
			}
			break;
		}
	}
}

void ASceneElement_Building_PolygonHelper::EntryShowevice(
	const FSceneElementConditional& ConditionalSet
	)
{
	SetActorHiddenInGame(false);
}

void ASceneElement_Building_PolygonHelper::EntryShowEffect(
	const FSceneElementConditional& ConditionalSet
	)
{
	SetActorHiddenInGame(false);
}

void ASceneElement_Building_PolygonHelper::QuitAllState()
{
	Super::QuitAllState();
	SetActorHiddenInGame(true);
}
