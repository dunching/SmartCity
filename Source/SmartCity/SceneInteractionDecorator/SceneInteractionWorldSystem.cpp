#include "SceneInteractionWorldSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Light.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetStringLibrary.h"

#include "Tools.h"
#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "GameplayTagsLibrary.h"
#include "LogWriter.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneInteractionDecorator.h"
#include "TemplateHelper.h"

USceneInteractionWorldSystem* USceneInteractionWorldSystem::GetInstance()
{
	return Cast<USceneInteractionWorldSystem>(
	                                          USubsystemBlueprintLibrary::GetWorldSubsystem(
		                                           GetWorldImp(),
		                                           USceneInteractionWorldSystem::StaticClass()
		                                          )
	                                         );
}

void USceneInteractionWorldSystem::SwitchInteractionMode(
	const FGameplayTag& Interaction_Mode
	)
{
	if (Interaction_Mode == UGameplayTagsLibrary::Interaction_Mode_Scene)
	{
	}
}

void USceneInteractionWorldSystem::SwitchViewArea(
	const FGameplayTag& Interaction_Area
	)
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

		DecoratorLayerAssetMap.Add(
		                           EDecoratorType::kArea,
		                           DecoratorSPtr
		                          );
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

		DecoratorLayerAssetMap.Add(
		                           EDecoratorType::kArea,
		                           DecoratorSPtr
		                          );
		return;
	}
}

void USceneInteractionWorldSystem::Operation(
	EOperatorType OperatorType
	) const
{
	for (const auto& Iter : DecoratorLayerAssetMap)
	{
		if (Iter.Value)
		{
			Iter.Value->Operation(OperatorType);
		}
	}
}

void USceneInteractionWorldSystem::UpdateFilter(
	EDecoratorType DecoratorType,
	const TSet<FGameplayTag>& FilterTags,
	const std::function<void(
		bool,
		const TSet<AActor*>&
		
		)>& OnEnd
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SceneObjSwitch>(
	                                                                        [this, OnEnd, DecoratorType, &FilterTags](
	                                                                        UGT_SceneObjSwitch* GTPtr
	                                                                        )
	                                                                        {
		                                                                        if (GTPtr)
		                                                                        {
			                                                                        GTPtr->
				                                                                        SceneInteractionWorldSystemPtr =
				                                                                        this;
			                                                                        GTPtr->DecoratorType =
				                                                                        DecoratorType;
			                                                                        GTPtr->FilterTags = FilterTags;
			                                                                        GTPtr->OnEnd.AddLambda(OnEnd);
		                                                                        }
	                                                                        }
	                                                                       );
}

void USceneInteractionWorldSystem::InitializeSceneActors()
{
	SCOPE_LOG_TIME_FUNC();

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_InitializeSceneActors>(
		 [this](
		 UGT_InitializeSceneActors* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->SceneInteractionWorldSystemPtr = this;
				 GTPtr->OnEnd.AddLambda(
				                        [this](
				                        bool
				                        )
				                        {
					                        SwitchViewArea(UGameplayTagsLibrary::Interaction_Area_ExternalWall);
				                        }
				                       );
			 }
		 }
		);
}

TWeakObjectPtr<AActor> USceneInteractionWorldSystem::FindSceneActor(
	const FGuid& ID
	) const
{
	if (ItemRefMap.Contains(ID))
	{
		return ItemRefMap[ID];
	}
	return nullptr;
}
