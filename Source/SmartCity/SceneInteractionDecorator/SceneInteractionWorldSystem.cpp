#include "SceneInteractionWorldSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Light.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetStringLibrary.h"
#include "GameFramework/SpringArmComponent.h"

#include "Tools.h"
#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "FloorHelper.h"
#include "GameplayCommand.h"
#include "GameplayTagsLibrary.h"
#include "IPSSI.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "RouteMarker.h"
#include "SceneElementBase.h"
#include "SceneElement_DeviceBase.h"
#include "SceneInteractionDecorator.h"
#include "SceneInteractionDecorator_Area.h"
#include "SceneInteractionDecorator_Mode.h"
#include "SmartCitySuiteTags.h"
#include "TemplateHelper.h"
#include "ViewTowerProcessor.h"
#include "ViewSingleFloorProcessor.h"
#include "TourPawn.h"
#include "ViewSingleDeviceProcessor.h"
#include "ViewSplitFloorProcessor.h"
#include "SceneElement_Space.h"
#include "ViewerPawnBase.h"
#include "WebChannelWorldSystem.h"

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
				DecoratorSPtr->SwitchIteractionType(EInteractionType::kDevice);

				NotifyOtherDecoratorsWhenEntry(Interaction_Mode, DecoratorSPtr);
			}
			return;
		}
		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Interaction_Space))
		{
			if (DecoratorSPtr)
			{
				DecoratorSPtr->SwitchIteractionType(EInteractionType::kSpace);

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
	const FGameplayTag& Interaction_Mode,
	const std::function<void(
		const TSharedPtr<FDecoratorBase>&
		
		)>& Func
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
		                                     USmartCitySuiteTags::Interaction_Mode_Empty,
		                                     Func
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
					                                        USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC,
					                                        Func
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
					                                            USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting,
					                                            Func
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
				                                                     USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR,
				                                                     Func
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
					                                         USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar,
					                                         Func
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
					                                                 USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_AccessControl,
					                                                 Func
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
				                                            USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator,
				                                            Func
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
				                                            USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow,
				                                            Func
				                                           );

				return;
			}
			if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_RollerBlind))
			{
				if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
				{
					if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
					    USmartCitySuiteTags::Interaction_Mode_DeviceManagger_RollerBlind)
					{
						return;
					}
				}

				SwitchDecoratorImp<FSunShadeMode_Decorator>(
				                                            USmartCitySuiteTags::Interaction_Mode,
				                                            USmartCitySuiteTags::Interaction_Mode_DeviceManagger_RollerBlind,
				                                            Func
				                                           );

				return;
			}
			SwitchDecoratorImp<FDeviceManaggerMode_Decorator>(
			                                                  USmartCitySuiteTags::Interaction_Mode,
			                                                  USmartCitySuiteTags::Interaction_Mode_DeviceManagger,
			                                                  Func
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
			                                         USmartCitySuiteTags::Interaction_Mode_Scene,
			                                         Func
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
			                                             USmartCitySuiteTags::Interaction_Mode_EmergencySystem,
			                                             Func
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
			                                          USmartCitySuiteTags::Interaction_Mode_EnergyManagement,
			                                          Func
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
			                                                           USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception,
			                                                           Func
			                                                          );

			return;
		}
		if (Interaction_Mode.MatchesTag(USmartCitySuiteTags::Interaction_Mode_BatchControl))
		{
			if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Mode))
			{
				if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Mode]->GetBranchDecoratorType() ==
				    USmartCitySuiteTags::Interaction_Mode_BatchControl)
				{
					// return;
				}
			}

			SwitchDecoratorImp<FBatchControlMode_Decorator>(
			                                                           USmartCitySuiteTags::Interaction_Mode,
			                                                           USmartCitySuiteTags::Interaction_Mode_BatchControl,
			                                                           Func
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

		SwitchDecoratorImp<FViewTower_Decorator>(
		                                         USmartCitySuiteTags::Interaction_Area,
		                                         Interaction_Area,
		                                         Func
		                                        );

		return;
	}

	if (Interaction_Area.MatchesTag(USmartCitySuiteTags::Interaction_Area_Periphery))
	{
		if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Area]->GetBranchDecoratorType() ==
			    USmartCitySuiteTags::Interaction_Area_Periphery)
			{
				return;
			}
		}

		SwitchDecoratorImp<FViewPeriphery_Decorator>(
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

	if (Interaction_Area.MatchesTag(USmartCitySuiteTags::Interaction_Area_Space))
	{
		if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Area]->GetBranchDecoratorType() ==
			    USmartCitySuiteTags::Interaction_Area_Space)
			{
				// return;
			}
		}

		SwitchDecoratorImp<FViewSpace_Decorator>(
		                                         USmartCitySuiteTags::Interaction_Area,
		                                         Interaction_Area,
		                                         Func
		                                        );

		return;
	}

	if (Interaction_Area.MatchesTag(USmartCitySuiteTags::Interaction_Area_SpecialArea))
	{
		if (DecoratorLayerAssetMap.Contains(USmartCitySuiteTags::Interaction_Area))
		{
			if (DecoratorLayerAssetMap[USmartCitySuiteTags::Interaction_Area]->GetBranchDecoratorType() ==
			    USmartCitySuiteTags::Interaction_Area_SpecialArea)
			{
				// return;
			}
		}

		SwitchDecoratorImp<FViewSpecialArea_Decorator>(
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

UGT_SwitchSceneElement_Base* USceneInteractionWorldSystem::UpdateFilter_Tower(
	const FSceneElementConditional& FilterTags,
	bool bBreakRuntimeTask,
	const TMulticastDelegate<void(
		bool,
		UGT_SwitchSceneElement_Base*

		
		)>& OnEnd
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	return PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSceneElement_Tower>(
		 bBreakRuntimeTask,
		 [this, OnEnd, &FilterTags](
		 UGT_SwitchSceneElement_Tower* GTPtr
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

UGT_SwitchSceneElement_Base* USceneInteractionWorldSystem::UpdateFilter_Floor(
	const FSceneElementConditional& FilterTags,
	bool bBreakRuntimeTask,
	const TMulticastDelegate<void(
		bool,
		UGT_SwitchSceneElement_Base*
		
		)>& OnEnd
	)
{
	if (FilterTags.ConditionalSet.HasTag(USmartCitySuiteTags::Interaction_Area_Floor_F12JF))
	{
		auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
		return PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSceneElement_Floor_JF>(
			 bBreakRuntimeTask,
			 [this, OnEnd, &FilterTags](
			 UGT_SwitchSceneElement_Floor_JF* GTPtr
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
	else
	{
		auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
		return PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSceneElement_Floor>(
			 bBreakRuntimeTask,
			 [this, OnEnd, &FilterTags](
			 UGT_SwitchSceneElement_Floor* GTPtr
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
}

UGT_SwitchSceneElement_Base* USceneInteractionWorldSystem::UpdateFilter_Space(
	const FSceneElementConditional& FilterTags,
	bool bBreakRuntimeTask,
	const TMulticastDelegate<void(
		bool,
		UGT_SwitchSceneElement_Base*


		
		)>& OnEnd,
		TWeakObjectPtr<ASceneElement_Space> SceneElementPtr,
		
			const TSet<TObjectPtr<ASceneElementBase>>& SkipSceneElementSet
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	return PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSceneElement_Space>(
		 bBreakRuntimeTask,
		 [this, OnEnd, &FilterTags, SceneElementPtr, SkipSceneElementSet](
		 UGT_SwitchSceneElement_Space* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->
					 SceneInteractionWorldSystemPtr =
					 this;
				 GTPtr->FilterTags = FilterTags;
				 GTPtr->SceneElementPtr = SceneElementPtr;
				 GTPtr->SkipSceneElementSet = SkipSceneElementSet;
				 GTPtr->OnEnd = OnEnd;
			 }
		 }
		);
}

UGT_SwitchSceneElement_Base* USceneInteractionWorldSystem::UpdateFilter_Device(
	const FSceneElementConditional& FilterTags,
	bool bBreakRuntimeTask,
	const TMulticastDelegate<void(
		bool,
		UGT_SwitchSceneElement_Base*


		
		)>& OnEnd,
	TWeakObjectPtr<ASceneElement_DeviceBase> SceneElementPtr
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	return PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSceneElement_Device>(
		 bBreakRuntimeTask,
		 [this, OnEnd, &FilterTags, SceneElementPtr](
		 UGT_SwitchSceneElement_Device* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->
					 SceneInteractionWorldSystemPtr =
					 this;
				 GTPtr->FilterTags = FilterTags;

				 GTPtr->SceneElementSet.Add(SceneElementPtr.Get());

				 GTPtr->FloorTag = SceneElementPtr->BelongFloor->FloorTag;

				 GTPtr->OnEnd = OnEnd;
			 }
		 }
		);
}

UGT_SwitchSceneElement_Base* USceneInteractionWorldSystem::UpdateFilter_BatchControlDevice(
	const FSceneElementConditional& FilterTags,
	bool bBreakRuntimeTask,
	const TMulticastDelegate<void(bool, UGT_SwitchSceneElement_Base*)>& OnEnd,
	TSet<TObjectPtr<ASceneElement_DeviceBase>> SceneElementSet,
	FGameplayTag FloorTag
	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	return PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSceneElement_BatchDevicesControl>(
		 bBreakRuntimeTask,
		 [this, OnEnd, &FilterTags, SceneElementSet, FloorTag](
		 UGT_SwitchSceneElement_BatchDevicesControl* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->
					 SceneInteractionWorldSystemPtr =
					 this;
			 	
				 GTPtr->FilterTags = FilterTags;

				 GTPtr->SceneElementSet = SceneElementSet;

				 GTPtr->FloorTag = FloorTag;

				 GTPtr->OnEnd = OnEnd;
			 }
		 }
		);
}

UGT_SwitchSceneElement_Base* USceneInteractionWorldSystem::UpdateFilter_SpeacialArea(
	const FSceneElementConditional& FilterTags,
	bool bBreakRuntimeTask,
	const TMulticastDelegate<void(
		bool,
		UGT_SwitchSceneElement_Base*

		
		)>& OnEnd,

	const TSet<FGameplayTag>& FloorSet,
	const TSet<FGameplayTag>& PriorityHideFloorSet

	)
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	return PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSceneElement_SpecialArea>(
		 bBreakRuntimeTask,
		 [this, OnEnd, &FilterTags, FloorSet, PriorityHideFloorSet](
		 UGT_SwitchSceneElement_SpecialArea* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->
					 SceneInteractionWorldSystemPtr =
					 this;
				 GTPtr->FilterTags = FilterTags;
				 GTPtr->OnEnd = OnEnd;

				 GTPtr->FloorSet = FloorSet;
				 GTPtr->PriorityHideFloorSet = PriorityHideFloorSet;
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
					                        USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
						                         USmartCitySuiteTags::Interaction_Area_ExternalWall
						                        );

					                        auto MessageSPtr = MakeShared<FMessageBody_UE_Initialized>();

					                        UWebChannelWorldSystem::GetInstance()->SendMessage(MessageSPtr);
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

void USceneInteractionWorldSystem::SetSceneActor(
	const FString& ID,
	ASceneElementBase* SceneElementPtr
	)
{
	SceneElementMap.Add(ID, SceneElementPtr);
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
	const TSet<ASceneElementBase*>& FocusActorsAry,
	const FSceneElementConditional& FilterTags
	)
{
	for (auto Iter : FocusActorsAry)
	{
		if (FocusActors.Contains(Iter))
		{
			return;
		}

		if (Iter)
		{
			Iter->SwitchInteractionType(FilterTags);

			FocusActors.Add(Iter);
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
