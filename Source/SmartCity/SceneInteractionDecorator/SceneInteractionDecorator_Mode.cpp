#include "SceneInteractionDecorator_Mode.h"

#include "Kismet/GameplayStatics.h"
#include "Net/WebChannelWorldSystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "GameOptions.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "SceneInteractionWorldSystem.h"
#include "Algorithm.h"
#include "DatasmithSceneActor.h"
#include "Dynamic_WeatherBase.h"
#include "FireMark.h"
#include "FloorHelper.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneElement_PWR_Pipe.h"
#include "TemplateHelper.h"
#include "FloorHelperBase.h"
#include "IPSSI.h"
#include "NavagationPaths.h"
#include "PersonMark.h"
#include "SceneElementCategory.h"
#include "SceneElement_Space.h"
#include "SmartCitySuiteTags.h"
#include "ViewSingleFloorProcessor.h"
#include "WeatherSystem.h"
#include "TourPawn.h"
#include "ViewTowerProcessor.h"
#include "ViewSingleDeviceProcessor.h"
#include "ViewSingleFloorViewEnergyProcessor.h"
#include "ViewSplitFloorProcessor.h"
#include "SceneElement_AccessControl.h"
#include "SceneInteractionDecorator_Area.h"

FEmpty_Decorator::FEmpty_Decorator() :
                                     Super(
                                          )
{
}

void FTour_Decorator::Entry()
{
	Super::Entry();
}

FTour_Decorator::FTour_Decorator() :
                                   Super(
                                        )
{
}

bool FTour_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	PRINTFUNCSTR();

	return Super::Operation(OperatorType);
}

FSceneMode_Decorator::FSceneMode_Decorator() :
                                             Super(
                                                  )
{
}

void FSceneMode_Decorator::Entry()
{
	Super::Entry();
}

bool FSceneMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	PRINTFUNCSTR();

	return Super::Operation(OperatorType);
}

FEmergencyMode_Decorator::FEmergencyMode_Decorator() :
                                                     Super(
                                                          )
{
}

void FEmergencyMode_Decorator::Entry()
{
	Super::Entry();

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );

	if (!AreaDecoratorSPtr)
	{
		return;
	}

	Spawn(AreaDecoratorSPtr);
}

void FEmergencyMode_Decorator::Quit()
{
	Clear();
	Super::Quit();
}

void FEmergencyMode_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (NewDecoratorSPtr)
	{
		Clear();
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area))
		{
			if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor))
			{
				auto AreaDecoratorSPtr =
					DynamicCastSharedPtr<FArea_Decorator>(NewDecoratorSPtr);

				Spawn(AreaDecoratorSPtr);
			}
		}
	}
}

void FEmergencyMode_Decorator::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorQuit(NewDecoratorSPtr);
}

void FEmergencyMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);
}

void FEmergencyMode_Decorator::Spawn(
	const TSharedPtr<FArea_Decorator>& AreaDecoratorSPtr
	)
{
	if (!AreaDecoratorSPtr)
	{
		return;
	}

	for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
		{
			Iter.Value->NavagationPaths->SwitchDisplay(
			                                           Iter.Value->GameplayTagContainer.HasTag(
				                                            AreaDecoratorSPtr->GetCurrentInteraction_Area()
				                                           )
			                                          );
		}

		if (Iter.Value->FloorTag == AreaDecoratorSPtr->GetBranchDecoratorType())
		{
			if (Iter.Value->SceneElementCategoryMap.Contains(USmartCitySuiteTags::SceneElement_Category_Space))
			{
				auto FireMarkClass = UAssetRefMap::GetInstance()->FireMarkClass;

				const auto Spaces = Iter.Value->SceneElementCategoryMap[
					USmartCitySuiteTags::SceneElement_Category_Space];
				TArray<AActor*> OutActors;

				Iter.Value->GetAttachedActors(OutActors, true, true);

				for (auto ActorIter : OutActors)
				{
					auto SceneElementBasePtr = Cast<ASceneElement_Space>(ActorIter);
					if (SceneElementBasePtr)
					{
						for (auto SpaceBoxIter : SceneElementBasePtr->CollisionComponentsAry)
						{
							if (FMath::RandRange(0, 100) > 60)
							{
								auto FireMarkPtr = GetWorldImp()->SpawnActor<AFireMark>(
									 FireMarkClass,
									 SpaceBoxIter->GetComponentLocation(),
									 FRotator::ZeroRotator
									);
								FireMarkSet.Add(FireMarkPtr);
							}
						}
					}
				}
			}
		}
	}
}

void FEmergencyMode_Decorator::Clear()
{
	for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
		{
			Iter.Value->NavagationPaths->SwitchDisplay(false);
		}
	}

	for (const auto& Iter : FireMarkSet)
	{
		if (Iter)
		{
			Iter->Destroy();
		}
	}
	FireMarkSet.Empty();
}

FEnvironmentalPerceptionMode_Decorator::FEnvironmentalPerceptionMode_Decorator() :
	Super(
	     )
{
}

FRadarMode_Decorator::FRadarMode_Decorator() :
                                             Super(
                                                  )
{
}

FRadarMode_Decorator::~FRadarMode_Decorator()
{
}

void FRadarMode_Decorator::Entry()
{
	Super::Entry();
}

void FRadarMode_Decorator::Quit()
{
	Super::Quit();
}

bool FRadarMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	return Super::Operation(OperatorType);
}

FDeviceManaggerMode_Decorator::FDeviceManaggerMode_Decorator() :
                                                               Super(
                                                                    )
{
}

FDeviceManaggerMode_Decorator::FDeviceManaggerMode_Decorator(
	FGameplayTag InBranchDecoratorType
	) :
	  Super(
	       )
{
}

FDeviceManaggerPWRMode_Decorator::FDeviceManaggerPWRMode_Decorator(
	) :
	  Super(
	        USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR
	       )
{
}

FDeviceManaggerPWRMode_Decorator::FDeviceManaggerPWRMode_Decorator(
	FGameplayTag InBranchDecoratorType
	) :
	  Super(
	        InBranchDecoratorType
	       )
{
}

FEnergyMode_Decorator::FEnergyMode_Decorator() :
                                               Super(
                                                    )
{
}

void FEnergyMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);
}

FHVACMode_Decorator::FHVACMode_Decorator() :
                                           Super(
                                                 USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC
                                                )
{
}

FLightingMode_Decorator::FLightingMode_Decorator() :
                                                   Super(
                                                         USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
                                                        )
{
}

void FLightingMode_Decorator::Entry()
{
	Super::Entry();
}

void FLightingMode_Decorator::Quit()
{
	Super::Quit();
}

void FLightingMode_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);
}

void FLightingMode_Decorator::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorQuit(NewDecoratorSPtr);
}

FAccessControlMode_Decorator::FAccessControlMode_Decorator() :
                                                             Super(
                                                                   USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_AccessControl
                                                                  )
{
}

void FAccessControlMode_Decorator::Entry()
{
	Super::Entry();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(), ASceneElement_AccessControl::StaticClass(), OutActors);

	USceneInteractionWorldSystem::GetInstance()->ClearFocus();
	USceneInteractionWorldSystem::GetInstance()->ClearRouteMarker();

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode);

	TSet<ASceneElementBase*> FocusActorsAry;

	for (auto Iter : OutActors)
	{
		auto SceneElementActorPtr = Cast<ASceneElementBase>(Iter);
		if (SceneElementActorPtr)
		{
			FocusActorsAry.Add(SceneElementActorPtr);
		}
	}
	USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(FocusActorsAry, SceneActorConditional);
}

FElevatorMode_Decorator::FElevatorMode_Decorator() :
                                                   Super(
                                                        )
{
}

void FElevatorMode_Decorator::Entry()
{
	Super::Entry();
}

void FElevatorMode_Decorator::Quit()
{
	Super::Quit();
}

void FElevatorMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);
}

FSunShadeMode_Decorator::FSunShadeMode_Decorator() :
                                                   Super(
                                                         USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow
                                                        )
{
}

FInteraction_Decorator::FInteraction_Decorator() :
                                                 Super(
                                                      )
{
	ControllConfig.CurrentWeather = WeatherSettings::Clear_Skies;

	ControllConfig.CurrentHour = 12;
}

void FInteraction_Decorator::Entry()
{
	Super::Entry();
}

FSingleDeviceMode_Decorator::FSingleDeviceMode_Decorator(
	) :
	  Super(
	       )

{
}

void FSingleDeviceMode_Decorator::Entry()
{
	Super::Entry();

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransformLocaterByID>(
		 false,
		 [this](
		 UGT_CameraTransformLocaterByID* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->TargetDevicePtr = TargetDevicePtr;
			 }
		 }
		);

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );

	if (!AreaDecoratorSPtr)
	{
		return;
	}

	const auto AreaTag = AreaDecoratorSPtr->GetCurrentInteraction_Area();
	if (UAssetRefMap::GetInstance()->FloorHelpers.Contains(AreaTag))
	{
		auto FloorPtr = UAssetRefMap::GetInstance()->FloorHelpers[AreaTag];
	}
}

FBatchControlMode_Decorator::FBatchControlMode_Decorator() :
	  Super(
		   )
{
}

void FBatchControlMode_Decorator::Entry()
{
	Super::Entry();

	Initial();
}

void FBatchControlMode_Decorator::ReEntry()
{
	Super::ReEntry();

	Initial();
}

void FBatchControlMode_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);
}

void FBatchControlMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);

	Process();
}

void FBatchControlMode_Decorator::Initial()
{
	SceneElementSet.Empty();
	
	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
											  USceneInteractionWorldSystem::GetInstance()->GetDecorator(
												   USmartCitySuiteTags::Interaction_Area
												  )
											 );

	if (!AreaDecoratorSPtr)
	{
		return;
	}
	

	if (
		AreaDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor)
	)
	{
		auto ViewFloor_DecoratorSPtr = DynamicCastSharedPtr<FFloor_Decorator>(AreaDecoratorSPtr);
		
		FloorTag = AreaDecoratorSPtr->GetBranchDecoratorType();

		for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (FloorIter.Value->GameplayTagContainer.HasTag(FloorTag))
			{
				auto MessageSPtr = MakeShared<FMessageBody_SelectedFloor>();

				for (auto Iter : FloorIter.Value->SceneElementCategoryMap)
				{
					if (!Iter.Key.MatchesTag(USmartCitySuiteTags::SceneElement_Category_Space))
					{
						continue;
					}

					TArray<AActor*> OutActors;
					Iter.Value->GetAttachedActors(OutActors, true, true);

					for (auto SpaceIter : OutActors)
					{
						auto SpacePtr = Cast<ASceneElement_Space>(SpaceIter);
						if (SpacePtr)
						{
							const auto DevicesSet = SpacePtr->GetAllDevices();
			
							for (const auto& DeviceIter : DevicesSet)
							{
								if (DeviceIter)
								{
									if (ExtensionParamMap.Contains(DeviceIter->DeviceTypeStr))
									{
										DeviceIter->UpdateExtensionParamMap(
																			ExtensionParamMap[DeviceIter->DeviceTypeStr],
																			false
																		   );
									}
									else
									{
										DeviceIter->UpdateExtensionParamMap(
																			{},
																			false
																		   );
									}
								}
							}
						}
					}
				}

				return;
			}
		}
	}

	if (
		AreaDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_Space)
	)
	{
		auto ViewSpace_DecoratorSPtr = DynamicCastSharedPtr<FViewSpace_Decorator>(AreaDecoratorSPtr);
		
		FloorTag = ViewSpace_DecoratorSPtr->SceneElementPtr->BelongFloor->FloorTag;

		const auto DevicesSet = ViewSpace_DecoratorSPtr->SceneElementPtr->GetAllDevices();
			
		SceneElementSet.Append(DevicesSet);
							
		for (const auto& DeviceIter : DevicesSet)
		{
			if (DeviceIter)
			{
				if (ExtensionParamMap.Contains(DeviceIter->DeviceTypeStr))
				{
					DeviceIter->UpdateExtensionParamMap(
														ExtensionParamMap[DeviceIter->DeviceTypeStr],
														false
													   );
				}
				else
				{
					DeviceIter->UpdateExtensionParamMap(
														{},
														false
													   );
				}
			}
		}

		return;
	}
}

void FBatchControlMode_Decorator::Process()
{
	FSceneElementConditional SceneActorConditional;

	TMulticastDelegate<void(
		bool,

		UGT_SwitchSceneElement_Base*


		
		)> MulticastDelegate;

	ON_SCOPE_EXIT
	{
		USceneInteractionWorldSystem::GetInstance()->UpdateFilter_BatchControlDevice(
																		SceneActorConditional,
																		true,
																		MulticastDelegate,
																		SceneElementSet,
																		FloorTag
																	   );
	};
}
