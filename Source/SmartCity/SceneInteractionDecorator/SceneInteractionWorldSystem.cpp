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
	if (Interaction_Mode == UGameplayTagsLibrary::Interaction_Mode_QD)
	{
		if (DecoratorLayerAssetMap.Contains(UGameplayTagsLibrary::Interaction_Mode))
		{
			if (DecoratorLayerAssetMap[UGameplayTagsLibrary::Interaction_Mode]->GetBranchDecoratorType() ==
			    UGameplayTagsLibrary::Interaction_Mode_QD)
			{
				return;
			}
		}

		SwitchDecoratorImp<FQDMode_Decorator>(
		                                      UGameplayTagsLibrary::Interaction_Mode,
		                                      UGameplayTagsLibrary::Interaction_Mode_QD
		                                     );

		return;
	}
	if (Interaction_Mode == UGameplayTagsLibrary::Interaction_Mode_Scene)
	{
		if (DecoratorLayerAssetMap.Contains(UGameplayTagsLibrary::Interaction_Mode))
		{
			if (DecoratorLayerAssetMap[UGameplayTagsLibrary::Interaction_Mode]->GetBranchDecoratorType() ==
			    UGameplayTagsLibrary::Interaction_Mode_Scene)
			{
				return;
			}
		}

		SwitchDecoratorImp<FSceneMode_Decorator>(
		                                      UGameplayTagsLibrary::Interaction_Mode,
		                                      UGameplayTagsLibrary::Interaction_Mode_Scene
		                                     );

		return;
	}
}

void USceneInteractionWorldSystem::SwitchViewArea(
	const FGameplayTag& Interaction_Area
	)
{
	if (Interaction_Area.MatchesTag(UGameplayTagsLibrary::Interaction_Area_ExternalWall))
	{
		if (DecoratorLayerAssetMap.Contains(UGameplayTagsLibrary::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[UGameplayTagsLibrary::Interaction_Area]->GetBranchDecoratorType() ==
			    UGameplayTagsLibrary::Interaction_Area_ExternalWall)
			{
				return;
			}
		}

		SwitchDecoratorImp<FExternalWall_Decorator>(
		                                            UGameplayTagsLibrary::Interaction_Area,
		                                            UGameplayTagsLibrary::Interaction_Area_ExternalWall,
		                                            Interaction_Area
		                                           );

		return;
	}

	if (Interaction_Area.MatchesTag(UGameplayTagsLibrary::Interaction_Area_Floor))
	{
		if (DecoratorLayerAssetMap.Contains(UGameplayTagsLibrary::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[UGameplayTagsLibrary::Interaction_Area]->GetBranchDecoratorType() ==
			    UGameplayTagsLibrary::Interaction_Area_Floor)
			{
				return;
			}
		}

		SwitchDecoratorImp<FFloor_Decorator>(
		                                     UGameplayTagsLibrary::Interaction_Area,
		                                     UGameplayTagsLibrary::Interaction_Area_Floor,
		                                     Interaction_Area
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
	const TSet<FSceneActorConditional,TSceneActorConditionalKeyFuncs>& FilterTags,
	const std::function<void(
		bool,
		const TSet<AActor*>&


		
		)>& OnEnd
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SceneObjSwitch>(
	                                                                        [this, OnEnd, &FilterTags](
	                                                                        UGT_SceneObjSwitch* GTPtr
	                                                                        )
	                                                                        {
		                                                                        if (GTPtr)
		                                                                        {
			                                                                        GTPtr->
				                                                                        SceneInteractionWorldSystemPtr =
				                                                                        this;
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

FGameplayTagContainer USceneInteractionWorldSystem::GetAllFilterTags() const
{
	FGameplayTagContainer Result;

	for (const auto& Iter : DecoratorLayerAssetMap)
	{
		if (Iter.Value)
		{
			Result.AddTag(Iter.Value->GetBranchDecoratorType());
		}
	}

	return Result;
}

void USceneInteractionWorldSystem::NotifyOtherDecorators(
	const FGameplayTag& MainTag, const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	) const
{
	for (const auto& Iter : DecoratorLayerAssetMap)
	{
		if (Iter.Key == MainTag)
		{
			continue;
		}
		if (Iter.Value)
		{
			Iter.Value->OnOtherDecoratorEntry(NewDecoratorSPtr);
		}
	}
}
