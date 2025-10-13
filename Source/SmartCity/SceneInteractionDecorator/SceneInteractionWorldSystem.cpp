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
#include "FloorHelper.h"
#include "GameplayCommand.h"
#include "GameplayTagsLibrary.h"
#include "IPSSI.h"
#include "LogWriter.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "RouteMarker.h"
#include "SceneElementBase.h"
#include "SceneInteractionDecorator.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewBuildingProcessor.h"
#include "ViewSingleFloorProcessor.h"
#include "TourPawn.h"
#include "ViewSingleDeviceProcessor.h"
#include "ViewSplitFloorProcessor.h"

USceneInteractionWorldSystem* USceneInteractionWorldSystem::GetInstance()
{
	return Cast<USceneInteractionWorldSystem>(
	                                          USubsystemBlueprintLibrary::GetWorldSubsystem(
		                                           GetWorldImp(),
		                                           USceneInteractionWorldSystem::StaticClass()
		                                          )
	                                         );
}

TSharedPtr<FDecoratorBase> USceneInteractionWorldSystem::GetInteractionModeDecorator() const
{
	if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
	{
		return DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode];
	}

	return nullptr;
}

TSharedPtr<FDecoratorBase> USceneInteractionWorldSystem::GetDecorator(
	const FGameplayTag& Interaction
	) const
{
	if (DecoratorLayerAssetMap.Contains(Interaction))
	{
		return DecoratorLayerAssetMap[Interaction];
	}

	return nullptr;
}

FGameplayTagContainer USceneInteractionWorldSystem::GetAllInteractionTags() const
{
	FGameplayTagContainer Result;
	for (const auto& Iter : DecoratorLayerAssetMap)
	{
		Result.AddTag(Iter.Value->GetBranchDecoratorType());
	}

	return Result;
}

void USceneInteractionWorldSystem::SwitchInteractionOption(
	const FGameplayTag& Interaction_Mode
	)
{
	if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Interaction))
	{
		auto DecoratorSPtr =
			DynamicCastSharedPtr<FInteraction_Decorator>(

			                                             GetDecorator(
			                                                          USmartCitySuiteTags::Interaction_Interaction
			                                                         )
			                                            );

		if (!DecoratorSPtr)
		{
			SwitchDecoratorImp<FInteraction_Decorator>(
			                                           USmartCitySuiteTags::Interaction_Interaction,
			                                           USmartCitySuiteTags::Interaction_Interaction
			                                          );
		}

		DecoratorSPtr =
			DynamicCastSharedPtr<FInteraction_Decorator>(

			                                             GetDecorator(
			                                                          USmartCitySuiteTags::Interaction_Interaction
			                                                         )
			                                            );

		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Interaction_Device))
		{
			if (DecoratorSPtr)
			{
				DecoratorSPtr->SwitchIteractionType(FInteraction_Decorator::EInteractionType::kDevice);

				NotifyOtherDecoratorsWhenEntry(Interaction_Mode, DecoratorSPtr);
			}
			return;
		}
		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Interaction_Space))
		{
			if (DecoratorSPtr)
			{
				DecoratorSPtr->SwitchIteractionType(FInteraction_Decorator::EInteractionType::kSpace);

				NotifyOtherDecoratorsWhenEntry(Interaction_Mode, DecoratorSPtr);
			}
			return;
		}
	}
}

void USceneInteractionWorldSystem::SetInteractionOption(
	const FGameplayTag& Interaction_Mode,
	const std::function<void(
		const TSharedPtr<FInteraction_Decorator>&


		
		)>& Func,
	bool bImmediatelyUpdate
	)
{
	if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Interaction))
	{
		auto DecoratorSPtr =
			DynamicCastSharedPtr<FInteraction_Decorator>(

			                                             GetDecorator(
			                                                          USmartCitySuiteTags::Interaction_Interaction
			                                                         )
			                                            );

		if (!DecoratorSPtr)
		{
			SwitchDecoratorImp<FInteraction_Decorator>(
			                                           USmartCitySuiteTags::Interaction_Interaction,
			                                           USmartCitySuiteTags::Interaction_Interaction
			                                          );
		}

		DecoratorSPtr =
			DynamicCastSharedPtr<FInteraction_Decorator>(

			                                             GetDecorator(
			                                                          USmartCitySuiteTags::Interaction_Interaction
			                                                         )
			                                            );

		if (Func)
		{
			Func(DecoratorSPtr);
		}

		if (bImmediatelyUpdate)
		{
			NotifyOtherDecoratorsWhenEntry(Interaction_Mode, DecoratorSPtr);
		}
	}
}

void USceneInteractionWorldSystem::SwitchInteractionMode(
	const FGameplayTag& Interaction_Mode
	)
{
	if (Interaction_Mode == FGameplayTag::EmptyTag || Interaction_Mode == USmartCitySuiteTags::Interaction_Mode_Empty)
	{
		if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
		{
			if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
			    USmartCitySuiteTags::Interaction_Mode_Empty)
			{
				return;
			}
		}

		SwitchDecoratorImp<FEmpty_Decorator>(
		                                     USmartCitySuiteTags::Interaction_Mode,
		                                     USmartCitySuiteTags::Interaction_Mode_Empty
		                                    );

		return;
	}

	if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode))
	{
		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger))
		{
			if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR))
			{
				if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC))
				{
					if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
					{
						if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
						    USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC)
						{
							return;
						}
					}

					SwitchDecoratorImp<FHVACMode_Decorator>(
					                                        USmartCitySuiteTags::Interaction_Mode,
					                                        USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC
					                                       );

					return;
				}

				if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting))
				{
					if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
					{
						if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
						    USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting)
						{
							return;
						}
					}

					SwitchDecoratorImp<FLightingMode_Decorator>(
					                                            USmartCitySuiteTags::Interaction_Mode,
					                                            USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
					                                           );

					return;
				}

				if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
				{
					if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
					    USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR)
					{
						return;
					}
				}

				SwitchDecoratorImp<FDeviceManaggerPWRMode_Decorator>(
				                                                     USmartCitySuiteTags::Interaction_Mode,
				                                                     USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR
				                                                    );

				return;
			}
			if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV))
			{
				if (Interaction_Mode == USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar)
				{
					if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
					{
						if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
						    USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar)
						{
							return;
						}
					}

					SwitchDecoratorImp<FRadarMode_Decorator>(
					                                         USmartCitySuiteTags::Interaction_Mode,
					                                         USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar
					                                        );

					return;
				}
				if (Interaction_Mode == USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_AccessControl)
				{
					if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
					{
						if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
						    USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_AccessControl)
						{
							return;
						}
					}

					SwitchDecoratorImp<FAccessControlMode_Decorator>(
					                                                 USmartCitySuiteTags::Interaction_Mode,
					                                                 USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_AccessControl
					                                                );

					return;
				}
				return;
			}
			if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator))
			{
				if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
				{
					if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
					    USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator)
					{
						return;
					}
				}

				SwitchDecoratorImp<FElevatorMode_Decorator>(
				                                            USmartCitySuiteTags::Interaction_Mode,
				                                            USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator
				                                           );

				return;
			}
			if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow))
			{
				if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
				{
					if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
					    USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow)
					{
						return;
					}
				}

				SwitchDecoratorImp<FSunShadeMode_Decorator>(
				                                            USmartCitySuiteTags::Interaction_Mode,
				                                            USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow
				                                           );

				return;
			}
			SwitchDecoratorImp<FDeviceManaggerMode_Decorator>(
			                                                  USmartCitySuiteTags::Interaction_Mode,
			                                                  USmartCitySuiteTags::Interaction_Mode_DeviceManagger
			                                                 );

			return;
		}
		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_Scene))
		{
			if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
			{
				if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
				    USmartCitySuiteTags::Interaction_Mode_Scene)
				{
					return;
				}
			}

			SwitchDecoratorImp<FSceneMode_Decorator>(
			                                         USmartCitySuiteTags::Interaction_Mode,
			                                         USmartCitySuiteTags::Interaction_Mode_Scene
			                                        );

			return;
		}
		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_EmergencySystem))
		{
			if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
			{
				if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
				    USmartCitySuiteTags::Interaction_Mode_EmergencySystem)
				{
					return;
				}
			}

			SwitchDecoratorImp<FEmergencyMode_Decorator>(
			                                             USmartCitySuiteTags::Interaction_Mode,
			                                             USmartCitySuiteTags::Interaction_Mode_EmergencySystem
			                                            );

			return;
		}
		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_EnergyManagement))
		{
			if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
			{
				if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
				    USmartCitySuiteTags::Interaction_Mode_EnergyManagement)
				{
					return;
				}
			}

			SwitchDecoratorImp<FEnergyMode_Decorator>(
			                                          USmartCitySuiteTags::Interaction_Mode,
			                                          USmartCitySuiteTags::Interaction_Mode_EnergyManagement
			                                         );

			return;
		}
		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception))
		{
			if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
			{
				if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
				    USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception)
				{
					return;
				}
			}

			SwitchDecoratorImp<FEnvironmentalPerceptionMode_Decorator>(
			                                                           USmartCitySuiteTags::Interaction_Mode,
			                                                           USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception
			                                                          );

			return;
		}
	}
}

void USceneInteractionWorldSystem::SwitchInteractionArea(
	const FGameplayTag& Interaction_Area,
	const std::function<void(

		const TSharedPtr<FDecoratorBase>&


		
		)>& Func
	)
{
	if (Interaction_Area.MatchesTag(USmartCitySuiteTags::Interaction_Area_ExternalWall))
	{
		if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Area]->GetBranchDecoratorType() ==
			    USmartCitySuiteTags::Interaction_Area_ExternalWall)
			{
				return;
			}
		}

		SwitchDecoratorImp<FExternalWall_Decorator>(
		                                            USmartCitySuiteTags::Interaction_Area,
		                                            Interaction_Area,
		                                            Func
		                                           );

		return;
	}

	if (Interaction_Area.MatchesTag(USmartCitySuiteTags::Interaction_Area_SplitFloor))
	{
		if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Area]->GetBranchDecoratorType() ==
			    USmartCitySuiteTags::Interaction_Area_SplitFloor)
			{
				return;
			}
		}

		SwitchDecoratorImp<FSplitFloor_Decorator>(
		                                          USmartCitySuiteTags::Interaction_Area,
		                                          Interaction_Area,
		                                          Func
		                                         );

		return;
	}

	if (Interaction_Area.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor))
	{
		if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Area]->GetBranchDecoratorType() ==
			    USmartCitySuiteTags::Interaction_Area_Floor)
			{
				return;
			}
		}

		SwitchDecoratorImp<FFloor_Decorator>(
		                                     USmartCitySuiteTags::Interaction_Area,
		                                     Interaction_Area,
		                                     Func
		                                    );

		return;
	}

	if (Interaction_Area.MatchesTag(USmartCitySuiteTags::Interaction_Area_ViewDevice))
	{
		if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Area]->GetBranchDecoratorType() ==
			    USmartCitySuiteTags::Interaction_Area_ViewDevice)
			{
				// return;
			}
		}

		SwitchDecoratorImp<FViewDevice_Decorator>(
		                                          USmartCitySuiteTags::Interaction_Area,
		                                          Interaction_Area,
		                                          Func
		                                         );

		return;
	}
}

void USceneInteractionWorldSystem::Operation(
	EOperatorType OperatorType
	) const
{
	// 如果有进行的任务，暂停操作
	if (Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorld()))->GameplayTasksComponentPtr->
		HasActiveTasks(UGameplayTask::StaticClass()))
	{
		return;
	}

	const auto TempDecoratorLayerAssetMap = DecoratorLayerAssetMap;
	for (const auto& Iter : TempDecoratorLayerAssetMap)
	{
		if (Iter.Value)
		{
			Iter.Value->Operation(OperatorType);
		}
	}
}

UGT_SwitchSceneElementState* USceneInteractionWorldSystem::UpdateFilter(
	const FSceneElementConditional& FilterTags,
	bool bBreakRuntimeTask,
	const TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&,
		UGT_SwitchSceneElementState*


		
		)>& OnEnd
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	return PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSceneElementState>(
		 bBreakRuntimeTask,
		 [this, OnEnd, &FilterTags](
		 UGT_SwitchSceneElementState* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->
					 SceneInteractionWorldSystemPtr =
					 this;
				 GTPtr->FilterTags = FilterTags;
				 GTPtr->OnEnd = OnEnd;
			 }
		 }
		);
}

void USceneInteractionWorldSystem::InitializeSceneActors()
{
	SCOPE_LOG_TIME_FUNC();

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_InitializeSceneActors>(
		 false,
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
					                        // SmartCityCommand::ReplyCameraTransform();

					                        UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
						                        TourProcessor::FViewBuildingProcessor>(
						                         [this](
						                         auto NewProcessor
						                         )
						                         {
						                         }
						                        );


					                        TArray<AActor*> OutActors;
					                        UGameplayStatics::GetAllActorsOfClass(
						                         this,
						                         AFloorHelper::StaticClass(),
						                         OutActors
						                        );
				                        }
				                       );
			 }
		 }
		);
}

TWeakObjectPtr<ASceneElementBase> USceneInteractionWorldSystem::FindSceneActor(
	const FString& ID
	) const
{
	if (SceneElementMap.Contains(ID))
	{
		return SceneElementMap[ID];
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

void USceneInteractionWorldSystem::SwitchInteractionType(
	AActor* DevicePtr,
	const FSceneElementConditional& ConditionalSet
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

	if (DevicePtr->IsA(ASceneElementBase::StaticClass()))
	{
		auto SceneElementBasePtr = Cast<ASceneElementBase>(DevicePtr);
		if (SceneElementBasePtr)
		{
			SceneElementBasePtr->SwitchInteractionType(ConditionalSet);
		}
	}
}

void USceneInteractionWorldSystem::AddFocusActor(
	AActor* ActorPtr
	)
{
	FocusActors.Add(ActorPtr);
}

void USceneInteractionWorldSystem::ClearFocus()
{
	for (auto Iter : FocusActors)
	{
		if (Iter)
		{
			if (Iter->IsA(ASceneElementBase::StaticClass()))
			{
				auto SceneElementBasePtr = Cast<ASceneElementBase>(Iter);
				if (SceneElementBasePtr)
				{
					SceneElementBasePtr->SwitchInteractionType(FSceneElementConditional::EmptyConditional);
				}
			}
			else
			{
				auto PrimitiveComponentPtr = Iter->GetComponentByClass<UPrimitiveComponent>();
				if (PrimitiveComponentPtr)
				{
					PrimitiveComponentPtr->SetRenderCustomDepth(false);
				}
			}
		}
	}

	FocusActors.Empty();
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
