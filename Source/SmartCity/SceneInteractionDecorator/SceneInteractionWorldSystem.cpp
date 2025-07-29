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
#include "InputProcessorSubSystem_Imp.h"
#include "LogWriter.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "RouteMarker.h"
#include "SceneInteractionDecorator.h"
#include "TemplateHelper.h"
#include "TourProcessor.h"
#include "ViewFloorProcessor.h"
#include "TourPawn.h"

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
	if (Interaction_Mode == FGameplayTag::EmptyTag)
	{
		if (DecoratorLayerAssetMap.Contains(UGameplayTagsLibrary::Interaction_Mode))
		{
			auto OldDecoratorSPtr = DecoratorLayerAssetMap[UGameplayTagsLibrary::Interaction_Mode];
			if (OldDecoratorSPtr)
			{
				OldDecoratorSPtr->Quit();
				NotifyOtherDecoratorsWhenQuit(OldDecoratorSPtr);
			}
		}

		return;
	}
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
	if (Interaction_Mode == UGameplayTagsLibrary::Interaction_Mode_AccessControl)
	{
		if (DecoratorLayerAssetMap.Contains(UGameplayTagsLibrary::Interaction_Mode))
		{
			if (DecoratorLayerAssetMap[UGameplayTagsLibrary::Interaction_Mode]->GetBranchDecoratorType() ==
			    UGameplayTagsLibrary::Interaction_Mode_AccessControl)
			{
				return;
			}
		}

		SwitchDecoratorImp<FAccessControlMode_Decorator>(
		                                         UGameplayTagsLibrary::Interaction_Mode,
		                                         UGameplayTagsLibrary::Interaction_Mode_AccessControl
		                                        );

		return;
	}
	if (Interaction_Mode == UGameplayTagsLibrary::Interaction_Mode_Elevator)
	{
		if (DecoratorLayerAssetMap.Contains(UGameplayTagsLibrary::Interaction_Mode))
		{
			if (DecoratorLayerAssetMap[UGameplayTagsLibrary::Interaction_Mode]->GetBranchDecoratorType() ==
			    UGameplayTagsLibrary::Interaction_Mode_Elevator)
			{
				return;
			}
		}

		SwitchDecoratorImp<FElevatorMode_Decorator>(
		                                         UGameplayTagsLibrary::Interaction_Mode,
		                                         UGameplayTagsLibrary::Interaction_Mode_Elevator
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

		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FTourProcessor>(
			 [](
			 auto NewProcessor
			 )
			 {
			 }
			);

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

		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewFloorProcessor>(
			 [](
			 auto NewProcessor
			 )
			 {
			 }
			);

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
	const TSet<FSceneElementConditional, TSceneElementConditionalKeyFuncs>& FilterTags,
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

FString USceneInteractionWorldSystem::GetName(
	AActor* DevicePtr
	) const
{
	if (DevicePtr)
	{
		auto Components = DevicePtr->GetComponents();
		for (auto SecondIter : Components)
		{
			auto InterfacePtr = Cast<IInterface_AssetUserData>(SecondIter);
			if (InterfacePtr)
			{
				auto AUDPtr = Cast<UDatasmithAssetUserData>(
															InterfacePtr->GetAssetUserDataOfClass(
																 UDatasmithAssetUserData::StaticClass()
																)
														   );
				if (!AUDPtr)
				{
					continue;
				}
				for (const auto& ThirdIter : AUDPtr->MetaData)
				{
					auto NamePrifixIter = UAssetRefMap::GetInstance()->NamePrifix.
																		   Find(ThirdIter.Key.ToString());
					if (NamePrifixIter)
					{
						return ThirdIter.Value;	
					}
				}
				break;
			}
		}
	}

	return TEXT("");
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

void USceneInteractionWorldSystem::NotifyOtherDecoratorsWhenEntry(
	const FGameplayTag& MainTag,
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
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

void USceneInteractionWorldSystem::NotifyOtherDecoratorsWhenQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	) const
{
	for (const auto& Iter : DecoratorLayerAssetMap)
	{
		if (Iter.Value)
		{
			Iter.Value->OnOtherDecoratorQuit(NewDecoratorSPtr);
		}
	}
}

void USceneInteractionWorldSystem::AddFocus(
	AActor* DevicePtr
	)
{
	if (!DevicePtr)
	{
		return;
	}
	if (FocusActors.Contains(DevicePtr))
	{
		return;
	}

	FocusActors.Add(DevicePtr);

	auto PrimitiveComponentPtr = DevicePtr->GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(true);
		PrimitiveComponentPtr->SetCustomDepthStencilValue(UGameOptions::GetInstance()->FocusOutline);
	}
}

void USceneInteractionWorldSystem::RemoveFocus(
	AActor* DevicePtr
	)
{
	if (!DevicePtr)
	{
		return;
	}
	if (!FocusActors.Contains(DevicePtr))
	{
		return;
	}

	auto PrimitiveComponentPtr = DevicePtr->GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComponentPtr)
	{
		PrimitiveComponentPtr->SetRenderCustomDepth(false);
	}

	FocusActors.Remove(DevicePtr);
}

void USceneInteractionWorldSystem::ClearFocus()
{
	for (auto Iter : FocusActors)
	{
		if (Iter)
		{
			auto PrimitiveComponentPtr = Iter->GetComponentByClass<UPrimitiveComponent>();
			if (PrimitiveComponentPtr)
			{
				PrimitiveComponentPtr->SetRenderCustomDepth(false);
			}
		}
	}
	FocusActors.Empty();
}

void USceneInteractionWorldSystem::AddRouteMarker(
	AActor* DevicePtr
	)
{
	if (!DevicePtr)
	{
		return;
	}
	if (RouteMarkers.Contains(DevicePtr))
	{
		return;
	}
	
	const auto Name = GetName(DevicePtr);
	if (Name.IsEmpty())
	{
		return;
	}
	auto RouteMarkerPtr = CreateWidget<URouteMarker>(
	                                                 GEngine->GetFirstLocalPlayerController(GetWorld()),
	                                                 UAssetRefMap::GetInstance()->RouteMarkerClass
	                                                );
	if (RouteMarkerPtr)
	{
		RouteMarkerPtr->TextStr = Name;
		RouteMarkerPtr->TargetActor = DevicePtr;
		RouteMarkerPtr->AddToViewport();
	}

	RouteMarkers.Add(DevicePtr, RouteMarkerPtr);
}

void USceneInteractionWorldSystem::RemoveRouteMarker(
	AActor* DevicePtr
	)
{
	if (!DevicePtr)
	{
		return;
	}
	if (!RouteMarkers.Contains(DevicePtr))
	{
		return;
	}

	if (RouteMarkers[DevicePtr])
	{
		RouteMarkers[DevicePtr]->RemoveFromParent();
	}

	RouteMarkers.Remove(DevicePtr);
}

void USceneInteractionWorldSystem::ClearRouteMarker()
{
	for (auto Iter : RouteMarkers)
	{
		if (Iter.Value)
		{
			Iter.Value->RemoveFromParent();
		}
	}
	RouteMarkers.Empty();
}
