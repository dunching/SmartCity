#include "SceneInteractionWorldSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"

#include "Tools.h"
#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "LogWriter.h"
#include "SceneInteractionDecorator.h"

USceneInteractionWorldSystem* USceneInteractionWorldSystem::GetInstance()
{
	return Cast<USceneInteractionWorldSystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(
			GetWorldImp(),
			USceneInteractionWorldSystem::StaticClass()
		)
	);
}

void USceneInteractionWorldSystem::SwitchInteractionMode(const FGameplayTag& Interaction_Mode)
{
	if (Interaction_Mode == UGameplayTagsLibrary::Interaction_Mode_Scene)
	{
	}
}

void USceneInteractionWorldSystem::SwitchViewArea(const FGameplayTag& Interaction_Area)
{
	if (Interaction_Area.MatchesTag(UGameplayTagsLibrary::Interaction_Area_ExternalWall))
	{
		if (DecoratorLayerAssetMap.Contains(EDecoratorType::kArea))
		{
			if (DecoratorLayerAssetMap[EDecoratorType::kArea]->GetBranchDecoratorType() ==
				EDecoratorType::kArea_ExternalWall)
			{
				return;
			}
		}

		auto DecoratorSPtr = MakeShared<FExternalWall_Decorator>(Interaction_Area);
		DecoratorSPtr->Entry();

		DecoratorLayerAssetMap.Add(EDecoratorType::kArea,
		                           DecoratorSPtr);
		return;
	}

	if (Interaction_Area.MatchesTag(UGameplayTagsLibrary::Interaction_Area_Floor))
	{
		if (DecoratorLayerAssetMap.Contains(EDecoratorType::kArea))
		{
			if (DecoratorLayerAssetMap[EDecoratorType::kArea]->GetBranchDecoratorType() ==
				EDecoratorType::kArea_Floor)
			{
				return;
			}
		}

		auto DecoratorSPtr = MakeShared<FFloor_Decorator>(Interaction_Area);
		DecoratorSPtr->Entry();

		DecoratorLayerAssetMap.Add(EDecoratorType::kArea,
		                           DecoratorSPtr);
		return;
	}
}
