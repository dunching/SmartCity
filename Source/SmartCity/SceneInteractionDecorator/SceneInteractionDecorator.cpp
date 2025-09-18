#include "SceneInteractionDecorator.h"

#include "SceneElement_AccessControl.h"
#include "Kismet/GameplayStatics.h"
#include "Net/WebChannelWorldSystem.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "GameOptions.h"
#include "LogWriter.h"
#include "MessageBody.h"
#include "SceneInteractionWorldSystem.h"
#include "Algorithm.h"
#include "Dynamic_WeatherBase.h"
#include "FloorHelper.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneElement_PWR_Pipe.h"
#include "TemplateHelper.h"
#include "FloorHelperBase.h"
#include "InputProcessorSubSystem_Imp.h"
#include "NavagationPaths.h"
#include "SceneElement_Space.h"
#include "SmartCitySuiteTags.h"
#include "ViewSingleFloorProcessor.h"
#include "WeatherSystem.h"
#include "TourPawn.h"
#include "ViewBuildingProcessor.h"
#include "ViewSingleFloorViewEnergyProcessor.h"
#include "ViewSplitFloorProcessor.h"

FDecoratorBase::FDecoratorBase(
	FGameplayTag InMainDecoratorType,
	FGameplayTag InBranchDecoratorType
	) :
	  MainDecoratorType(InMainDecoratorType)
	  , BranchDecoratorType(InBranchDecoratorType)
{
}

FDecoratorBase::~FDecoratorBase()
{
}

void FDecoratorBase::Entry()
{
}

void FDecoratorBase::Quit()
{
}

bool FDecoratorBase::NeedAsync() const
{
	return false;
}

void FDecoratorBase::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
}

void FDecoratorBase::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
}

bool FDecoratorBase::Operation(
	EOperatorType OperatorType
	)
{
	return false;
}

FGameplayTag FDecoratorBase::GetMainDecoratorType() const
{
	return MainDecoratorType;
}

FGameplayTag FDecoratorBase::GetBranchDecoratorType() const
{
	return BranchDecoratorType;
}

void FDecoratorBase::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElementState* TaskPtr
	)
{
}

void FDecoratorBase::IncreaseWaitTaskCount()
{
	WaitTaskCount++;
}

void FDecoratorBase::DecreaseWaitTaskCount()
{
	WaitTaskCount--;
}

int32 FDecoratorBase::GetWaitTaskCount() const
{
	return WaitTaskCount;
}

FEmpty_Decorator::FEmpty_Decorator() :
                                     Super(
                                           USmartCitySuiteTags::Interaction_Mode,
                                           USmartCitySuiteTags::Interaction_Mode_Empty
                                          )
{
}

void FTour_Decorator::Entry()
{
	Super::Entry();
}

FTour_Decorator::FTour_Decorator() :
                                   Super(
                                         USmartCitySuiteTags::Interaction_Mode,
                                         USmartCitySuiteTags::Interaction_Mode_Tour
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
                                                   USmartCitySuiteTags::Interaction_Mode,
                                                   USmartCitySuiteTags::Interaction_Mode_Scene
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
                                                           USmartCitySuiteTags::Interaction_Mode,
                                                           USmartCitySuiteTags::Interaction_Mode_EmergencySystem
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

	for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
		{
			Iter.Value->NavagationPaths->SwitchDisplay(
			                                           AreaDecoratorSPtr->GetCurrentInteraction_Area().MatchesTag(
				                                            Iter.Value->FloorTag
				                                           )
			                                          );
		}
	}
}

void FEmergencyMode_Decorator::Quit()
{
	for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
		{
			Iter.Value->NavagationPaths->SwitchDisplay(false);
		}
	}

	Super::Quit();
}

void FEmergencyMode_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (NewDecoratorSPtr)
	{
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor))
		{
			auto AreaDecoratorSPtr =
				DynamicCastSharedPtr<FArea_Decorator>(NewDecoratorSPtr);

			if (!AreaDecoratorSPtr)
			{
				return;
			}

			for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
			{
				if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
				{
					Iter.Value->NavagationPaths->SwitchDisplay(
					                                           AreaDecoratorSPtr->GetCurrentInteraction_Area().
					                                           MatchesTag(
					                                                      Iter.Value->FloorTag
					                                                     )
					                                          );
				}
			}
		}
		else
		{
			for (const auto& Iter : UAssetRefMap::GetInstance()->FloorHelpers)
			{
				if (Iter.Value->NavagationPaths.ToSoftObjectPath().IsValid())
				{
					Iter.Value->NavagationPaths->SwitchDisplay(false);
				}
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
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElementState* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors, TaskPtr);
}

FEnvironmentalPerceptionMode_Decorator::FEnvironmentalPerceptionMode_Decorator() :
	Super(
	      USmartCitySuiteTags::Interaction_Mode,
	      USmartCitySuiteTags::Interaction_Mode_EnvironmentalPerception
	     )
{
}

FELVRadarMode_Decorator::FELVRadarMode_Decorator() :
                                                   Super(
                                                         USmartCitySuiteTags::Interaction_Mode,
                                                         USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar
                                                        )
{
}

FELVRadarMode_Decorator::~FELVRadarMode_Decorator()
{
}

void FELVRadarMode_Decorator::Entry()
{
	Super::Entry();
}

void FELVRadarMode_Decorator::Quit()
{
	Super::Quit();
}

bool FELVRadarMode_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	return Super::Operation(OperatorType);
}

FDeviceManaggerMode_Decorator::FDeviceManaggerMode_Decorator() :
                                                               Super(
                                                                     USmartCitySuiteTags::Interaction_Mode,
                                                                     USmartCitySuiteTags::Interaction_Mode_DeviceManagger
                                                                    )
{
}

FDeviceManaggerMode_Decorator::FDeviceManaggerMode_Decorator(
	FGameplayTag InBranchDecoratorType
	) :
	  Super(
	        USmartCitySuiteTags::Interaction_Mode,
	        InBranchDecoratorType
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
                                                     USmartCitySuiteTags::Interaction_Mode,
                                                     USmartCitySuiteTags::Interaction_Mode_EnergyManagement
                                                    )
{
}

void FEnergyMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElementState* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors, TaskPtr);

	for (auto Iter : InActors)
	{
		auto PipePtr = Cast<ASceneElement_PWR_Pipe>(Iter);
		if (PipePtr)
		{
			PipeActors.Add(PipePtr);

			continue;
		}
		
		auto DevicePtr = Cast<ASceneElement_DeviceBase>(Iter);
		if (DevicePtr)
		{
			OtherDevices.Add(DevicePtr);

			continue;
		}
	}

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(AreaDecoratorSPtr->GetBranchDecoratorType());
	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	for (auto Iter : PipeActors)
	{
		if (Iter)
		{
			Iter->SwitchInteractionType(SceneActorConditional);
			
			IDMap.Add(Iter->GetID(), Iter);
		}
	}

	for (auto Iter : OtherDevices)
	{
		if (Iter)
		{
			Iter->SwitchInteractionType(SceneActorConditional);
		}
	}
}

FPWRHVACMode_Decorator::FPWRHVACMode_Decorator() :
                                                 Super(
                                                       USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_HVAC
                                                      )
{
}

FPWRLightingMode_Decorator::FPWRLightingMode_Decorator() :
                                                         Super(
                                                               USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
                                                              )
{
}

void FPWRLightingMode_Decorator::Entry()
{
	Super::Entry();

	auto AreaDecoratorSPtr =
		DynamicCastSharedPtr<FArea_Decorator>(
		                                      USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			                                       USmartCitySuiteTags::Interaction_Area
			                                      )
		                                     );
	if (AreaDecoratorSPtr && AreaDecoratorSPtr->GetCurrentInteraction_Area().MatchesTag(
		     USmartCitySuiteTags::Interaction_Area_Floor
		    ))
	{
		UWeatherSystem::GetInstance()->AdjustTime(FDateTime(1, 1, UAssetRefMap::GetInstance()->ViewLightingTime));
	}
	else
	{
	}
}

void FPWRLightingMode_Decorator::Quit()
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
		UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(DecoratorSPtr->GetCurrentWeather());

		FDateTime Time(1, 1, 1, DecoratorSPtr->GetCurrentHour());
		UWeatherSystem::GetInstance()->AdjustTime(Time);
	}

	Super::Quit();
}

void FPWRLightingMode_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor))
	{
		UWeatherSystem::GetInstance()->AdjustTime(FDateTime(1, 1, UAssetRefMap::GetInstance()->ViewLightingTime));
	}
	else
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
			UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(DecoratorSPtr->GetCurrentWeather());

			FDateTime Time(1, 1, 1, DecoratorSPtr->GetCurrentHour());
			UWeatherSystem::GetInstance()->AdjustTime(Time);
		}
	}
}

void FPWRLightingMode_Decorator::OnOtherDecoratorQuit(
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

	for (auto Iter : OutActors)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(Iter, SceneActorConditional);
	}
}

FElevatorMode_Decorator::FElevatorMode_Decorator() :
                                                   Super(
                                                         USmartCitySuiteTags::Interaction_Mode,
                                                         USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator
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
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElementState* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors, TaskPtr);
}

FSunShadeMode_Decorator::FSunShadeMode_Decorator() :
                                                   Super(
                                                         USmartCitySuiteTags::Interaction_Mode_DeviceManagger_SunShadow
                                                        )
{
}

FArea_Decorator::FArea_Decorator(
	const FGameplayTag& Interaction_Area
	) :
	  Super(
	        USmartCitySuiteTags::Interaction_Area,
	        Interaction_Area
	       )
	  , CurrentInteraction_Area(Interaction_Area)
{
}

void FArea_Decorator::Entry()
{
	Super::Entry();
}

void FArea_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);
}

void FArea_Decorator::OnOtherDecoratorQuit(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorQuit(NewDecoratorSPtr);
}

FGameplayTag FArea_Decorator::GetCurrentInteraction_Area() const
{
	return CurrentInteraction_Area;
}

void FArea_Decorator::UpdateDisplay()
{
}

void FArea_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElementState* TaskPtr
	)
{
}

FExternalWall_Decorator::FExternalWall_Decorator(
	const FGameplayTag& Interaction_Area
	) :
	  Super(
	        Interaction_Area
	       )
{
}

void FExternalWall_Decorator::Entry()
{
	Super::Entry();
	
	ON_SCOPE_EXIT
	{
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewBuildingProcessor>(
			 [](
			 auto NewProcessor
			 )
			 {
			 }
			);
	};
	
	{
		auto DecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			 USmartCitySuiteTags::Interaction_Mode
			);
		if (
			DecoratorSPtr
		)
		{
			if (
				DecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode)
			)
			{
				if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode_Empty))
				{
					FSceneElementConditional SceneActorConditional;

					SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

					TMulticastDelegate<void(
						bool,
						const TSet<AActor*>&,
						UGT_SwitchSceneElementState*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
					                                                          SceneActorConditional,
					                                                          true,
					                                                          MulticastDelegate
					                                                         );

					return;
				}
				else if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
				                                                            USmartCitySuiteTags::Interaction_Mode_EmergencySystem
				                                                           ))
				{
					FSceneElementConditional SceneActorConditional;

					SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

					TMulticastDelegate<void(
						bool,
						const TSet<AActor*>&,
						UGT_SwitchSceneElementState*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
					                                                          SceneActorConditional,
					                                                          true,
					                                                          MulticastDelegate
					                                                         );

					return;
				}
				else if (DecoratorSPtr->GetBranchDecoratorType().
				                        MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator))
				{
					FSceneElementConditional SceneActorConditional;

					SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
					SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

					TMulticastDelegate<void(
						bool,
						const TSet<AActor*>&,
						UGT_SwitchSceneElementState*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
					                                                          SceneActorConditional,
					                                                          true,
					                                                          MulticastDelegate
					                                                         );

					return;
				}
				{
					FSceneElementConditional SceneActorConditional;

					SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

					TMulticastDelegate<void(
						bool,
						const TSet<AActor*>&,
						UGT_SwitchSceneElementState*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
					                                                          SceneActorConditional,
					                                                          true,
					                                                          MulticastDelegate
					                                                         );

					return;
				}
			}
		}
		else
		{
		}
	}
	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&,
		UGT_SwitchSceneElementState*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
	                                                          true,
	                                                          MulticastDelegate
	                                                         );
}

void FExternalWall_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode)
	)
	{
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode_Empty))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );
			return;
		}
		else if (NewDecoratorSPtr->GetBranchDecoratorType().
		                           MatchesTag(USmartCitySuiteTags::Interaction_Mode_EmergencySystem))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );
			return;
		}

		else if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(
		                                                               USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator
		                                                              ))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );
			return;
		}
		else
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());


			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );
			return;
		}
	}
	else
	{
	}

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&,
		UGT_SwitchSceneElementState*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
	                                                          true,
	                                                          MulticastDelegate
	                                                         );
}

bool FExternalWall_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	Super::Operation(OperatorType);

	PRINTFUNCSTR();

	switch (OperatorType)
	{
	case EOperatorType::kLeftMouseButton:
	case EOperatorType::kRightMouseButton:
		{
		}
		break;
	case EOperatorType::kNone:
		break;
	default: ;
	}

	return false;
}

void FExternalWall_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElementState* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors, TaskPtr);
}

FSplitFloor_Decorator::FSplitFloor_Decorator(
	const FGameplayTag& Interaction_Area
	) :
	  Super(
	        USmartCitySuiteTags::Interaction_Area,
	        Interaction_Area
	       )
{
}

void FSplitFloor_Decorator::Entry()
{
	Super::Entry();

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_FloorSplit>(
	                                                                    true,
	                                                                    [this](
	                                                                    UGT_FloorSplit* GTPtr
	                                                                    )
	                                                                    {
	                                                                    }
	                                                                    );
	
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewSplitFloorProcessor>(
		 [](
		 auto NewProcessor
		 )
		 {
		 }
		);

}

void FSplitFloor_Decorator::Quit()
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_QuitFloorSplit>(
	                                                                        true,
	                                                                        [this](
	                                                                        auto GTPtr
	                                                                        )
	                                                                        {
		                                                                        GTPtr->OnEnd.AddLambda(
			                                                                         [this](
			                                                                         auto
			                                                                         )
			                                                                         {
				                                                                         OnAsyncQuitComplete.
					                                                                         ExecuteIfBound();
			                                                                         }
			                                                                        );
	                                                                        }
	                                                                       );

	Super::Quit();
}

void FSplitFloor_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);
}

bool FSplitFloor_Decorator::NeedAsync() const
{
	return true;
}

bool FSplitFloor_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	return Super::Operation(OperatorType);
}

FFloor_Decorator::FFloor_Decorator(
	const FGameplayTag& Interaction_Area
	) :
	  Super(
	        Interaction_Area
	       )
{
}

FFloor_Decorator::~FFloor_Decorator()
{
}

void FFloor_Decorator::Entry()
{
	Super::Entry();

	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(WeatherSettings::Clear_Skies);

	FDateTime Time(1, 1, 1, 12);
	UWeatherSystem::GetInstance()->AdjustTime(Time);

	auto DecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->GetDecorator(
		 USmartCitySuiteTags::Interaction_Mode
		);
	if (
		DecoratorSPtr
	)
	{
		if (
			DecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode)
		)
		{
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode_Empty))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&,
					UGT_SwitchSceneElementState*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          true,
				                                                          MulticastDelegate
				                                                         );

				IncreaseWaitTaskCount();
				
				UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
					TourProcessor::FViewSingleFloorProcessor>(
																		[this](
																		auto NewProcessor
																		)
																		{
																			NewProcessor->Interaction_Area =
																				GetBranchDecoratorType();
																		}
																	   );

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().
			                   MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&,
					UGT_SwitchSceneElementState*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          true,
				                                                          MulticastDelegate
				                                                         );

				IncreaseWaitTaskCount();
				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_EmergencySystem
			                                                      ))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&,
					UGT_SwitchSceneElementState*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          true,
				                                                          MulticastDelegate
				                                                         );

				IncreaseWaitTaskCount();
				
				UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
					TourProcessor::FViewSingleFloorProcessor>(
																		[this](
																		auto NewProcessor
																		)
																		{
																			NewProcessor->Interaction_Area =
																				GetBranchDecoratorType();
																		}
																	   );

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_EnergyManagement
			                                                      ))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&,
					UGT_SwitchSceneElementState*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          true,
				                                                          MulticastDelegate
				                                                         );

				IncreaseWaitTaskCount();
				
				UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
					TourProcessor::FViewSingleFloorViewEnergyProcessor>(
																		[this](
																		auto NewProcessor
																		)
																		{
																			NewProcessor->Interaction_Area =
																				GetBranchDecoratorType();
																		}
																	   );

				return;
			}
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&,
					UGT_SwitchSceneElementState*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          true,
				                                                          MulticastDelegate
				                                                         );

				IncreaseWaitTaskCount();
				return;
			}
		}
	}
	else
	{
	}

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&,
		UGT_SwitchSceneElementState*
		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
	                                                          true,
	                                                          MulticastDelegate
	                                                         );

	IncreaseWaitTaskCount();
	
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
		TourProcessor::FViewSingleFloorProcessor>(
															[this](
															auto NewProcessor
															)
															{
																NewProcessor->Interaction_Area =
																	GetBranchDecoratorType();
															}
														   );

}

void FFloor_Decorator::Quit()
{
	USceneInteractionWorldSystem::GetInstance()->ClearFocus();
	USceneInteractionWorldSystem::GetInstance()->ClearRouteMarker();

	Super::Quit();
}

void FFloor_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode)
	)
	{
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode_Empty))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*
				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );

			IncreaseWaitTaskCount();
			
			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
				TourProcessor::FViewSingleFloorProcessor>(
																	[this](
																	auto NewProcessor
																	)
																	{
																		NewProcessor->Interaction_Area =
																			GetBranchDecoratorType();
																	}
																   );

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(
		                                                          USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator
		                                                         ))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*
				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );

			IncreaseWaitTaskCount();
			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_EmergencySystem))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*
				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );

			IncreaseWaitTaskCount();
			
			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
				TourProcessor::FViewSingleFloorProcessor>(
																	[this](
																	auto NewProcessor
																	)
																	{
																		NewProcessor->Interaction_Area =
																			GetBranchDecoratorType();
																	}
																   );

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_EnergyManagement))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*
				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
			MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );

			IncreaseWaitTaskCount();
			
			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
				TourProcessor::FViewSingleFloorViewEnergyProcessor>(
																	[ this](
																	auto NewProcessor
																	)
																	{
																		NewProcessor->Interaction_Area =
																			GetBranchDecoratorType();
																	}
																   );

			return;
		}
		
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&,
				UGT_SwitchSceneElementState*
				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          true,
			                                                          MulticastDelegate
			                                                         );

			IncreaseWaitTaskCount();
			
			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
				TourProcessor::FViewSingleFloorProcessor>(
																	[this](
																	auto NewProcessor
																	)
																	{
																		NewProcessor->Interaction_Area =
																			GetBranchDecoratorType();
																	}
																   );

			return;
		}
	}
	else
	{
	}

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Interaction)
	)
	{
		FSceneElementConditional SceneActorConditional;

		SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
		auto DecoratorSPtr = USceneInteractionWorldSystem::GetInstance()->GetDecorator(
			 USmartCitySuiteTags::Interaction_Mode
			);
		if (
			DecoratorSPtr
		)
		{
			SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());
		}

		TMulticastDelegate<void(
			bool,
			const TSet<AActor*>&,
			UGT_SwitchSceneElementState*
			
			)> MulticastDelegate;

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
		                                                          SceneActorConditional,
		                                                          true,
		                                                          MulticastDelegate
		                                                         );

		IncreaseWaitTaskCount();
		
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
			TourProcessor::FViewSingleFloorProcessor>(
																[this](
																auto NewProcessor
																)
																{
																	NewProcessor->Interaction_Area =
																		GetBranchDecoratorType();
																}
															   );

		return;
	}

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet = USceneInteractionWorldSystem::GetInstance()->GetAllInteractionTags();

	TMulticastDelegate<void(
		bool,
		const TSet<AActor*>&,
		UGT_SwitchSceneElementState*
		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
	                                                          true,
	                                                          MulticastDelegate
	                                                         );

	IncreaseWaitTaskCount();
	
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
		TourProcessor::FViewSingleFloorProcessor>(
															[this](
															auto NewProcessor
															)
															{
																NewProcessor->Interaction_Area =
																	GetBranchDecoratorType();
															}
														   );

}

bool FFloor_Decorator::Operation(
	EOperatorType OperatorType
	)
{
	Super::Operation(OperatorType);

	switch (OperatorType)
	{
	case EOperatorType::kLeftMouseButton:
	case EOperatorType::kRightMouseButton:
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
				case FInteraction_Decorator::EInteractionType::kDevice:
					{
						TArray<struct FHitResult> OutHits;

						auto PCPtr = Cast<APlanetPlayerController>(
						                                           GEngine->GetFirstLocalPlayerController(GetWorldImp())
						                                          );

						FVector2D MousePosition;
						PCPtr->GetMousePosition(
						                        MousePosition.X,
						                        MousePosition.Y
						                       );

						FVector WorldLocation;
						FVector WorldDirection;
						PCPtr->DeprojectScreenPositionToWorld(
						                                      MousePosition.X,
						                                      MousePosition.Y,
						                                      WorldLocation,
						                                      WorldDirection
						                                     );

						// 优先检测设备
						{
							FCollisionObjectQueryParams ObjectQueryParams;
							ObjectQueryParams.AddObjectTypesToQuery(Device_Object);
							GetWorldImp()->LineTraceMultiByObjectType(
							                                          OutHits,
							                                          WorldLocation,
							                                          WorldLocation + (
								                                          WorldDirection * UGameOptions::GetInstance()
								                                          ->LinetraceDistance),
							                                          ObjectQueryParams
							                                         );

							for (const auto& Iter : OutHits)
							{
								if (Iter.GetActor())
								{
									if (Iter.GetActor()->IsHidden())
									{
										continue;
									}

									{
										FSceneElementConditional SceneActorConditional;

										SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
										SceneActorConditional.ConditionalSet.AddTag(
											 DecoratorSPtr->GetBranchDecoratorType()
											);

										for (auto PreviousActorsIter : PreviousActors)
										{
											USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(
												 PreviousActorsIter,
												 SceneActorConditional
												);
										}
										PreviousActors.Empty();
									}

									FSceneElementConditional SceneActorConditional;

									SceneActorConditional.ConditionalSet.AddTag(
									                                            USmartCitySuiteTags::Interaction_Mode_View
									                                           );

									PreviousActors.Add(Iter.GetActor());

									USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(
										 Iter.GetActor(),
										 SceneActorConditional
										);

									return true;
								}
							}

							// 取消选择
							USceneInteractionWorldSystem::GetInstance()->SwitchInteractionMode(FGameplayTag::EmptyTag);
							// UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
							// 	TourProcessor::FViewSingleFloorProcessor>(
							// 	                                          [this](
							// 	                                          auto NewProcessor
							// 	                                          )
							// 	                                          {
							// 		                                          NewProcessor->Interaction_Area =
							// 			                                          GetCurrentInteraction_Area();
							// 	                                          }
							// 	                                         );
						}
					}
					break;
				case FInteraction_Decorator::EInteractionType::kSpace:
					{
						TArray<struct FHitResult> OutHits;

						auto PCPtr = Cast<APlanetPlayerController>(
						                                           GEngine->GetFirstLocalPlayerController(GetWorldImp())
						                                          );

						FVector2D MousePosition;
						PCPtr->GetMousePosition(
						                        MousePosition.X,
						                        MousePosition.Y
						                       );

						FVector WorldLocation;
						FVector WorldDirection;
						PCPtr->DeprojectScreenPositionToWorld(
						                                      MousePosition.X,
						                                      MousePosition.Y,
						                                      WorldLocation,
						                                      WorldDirection
						                                     );

						// 检测区域
						{
							FCollisionObjectQueryParams ObjectQueryParams;
							ObjectQueryParams.AddObjectTypesToQuery(Space_Object);
							GetWorldImp()->LineTraceMultiByObjectType(
							                                          OutHits,
							                                          WorldLocation,
							                                          WorldLocation + (
								                                          WorldDirection * UGameOptions::GetInstance()
								                                          ->LinetraceDistance),
							                                          ObjectQueryParams
							                                         );

							for (const auto& Iter : OutHits)
							{
								if (Iter.GetActor())
								{
									if (Iter.GetActor()->IsHidden())
									{
										continue;
									}

									auto SpaceElementPtr = Cast<ASceneElement_Space>(Iter.GetActor());
									if (!SpaceElementPtr)
									{
										continue;
									}

									{
										FSceneElementConditional SceneActorConditional;

										SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
										SceneActorConditional.ConditionalSet.AddTag(
											 DecoratorSPtr->GetBranchDecoratorType()
											);

										for (auto PreviousActorsIter : PreviousActors)
										{
											USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(
												 PreviousActorsIter,
												 SceneActorConditional
												);
										}
										PreviousActors.Empty();
									}

									FSceneElementConditional SceneActorConditional;

									SceneActorConditional.ConditionalSet.AddTag(
									                                            USmartCitySuiteTags::Interaction_Mode_Focus
									                                           );

									USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(
										 Iter.GetActor(),
										 SceneActorConditional
										);

									PreviousActors.Add(Iter.GetActor());

									PCPtr->GameplayTasksComponentPtr->StartGameplayTask<
										UGT_CameraTransformLocaterBySpace>(
										                                   false,
										                                   [&Iter](
										                                   UGT_CameraTransformLocaterBySpace* GTPtr
										                                   )
										                                   {
											                                   if (GTPtr)
											                                   {
												                                   GTPtr->SpaceActorPtr = Iter.
													                                   GetActor();
											                                   }
										                                   }
										                                  );

									return true;
								}
							}
						}
					}
					break;
				}
			}
			else
			{
			}
		}
		break;
	case EOperatorType::kNone:
		break;
	default: ;
	}

	USceneInteractionWorldSystem::GetInstance()->ClearFocus();

	return false;
}

void FFloor_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElementState* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors, TaskPtr);

	DecreaseWaitTaskCount();

	auto Result = UKismetAlgorithm::GetCameraSeat(
	                                              InActors,
	                                              UGameOptions::GetInstance()->ViewFloorControlParam.ViewRot,
	                                              UGameOptions::GetInstance()->ViewFloorControlParam.FOV
	                                             );

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_ModifyCameraTransform>(
		 false,
		 [Result](
		 UGT_ModifyCameraTransform* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->TargetLocation = Result.Key.GetLocation();
				 GTPtr->TargetRotation = Result.Key.GetRotation().Rotator();
				 GTPtr->TargetTargetArmLength = Result.Value;
			 }
		 }
		);
}

FInteraction_Decorator::FInteraction_Decorator() :
                                                 Super(
                                                       USmartCitySuiteTags::Interaction_Interaction,
                                                       USmartCitySuiteTags::Interaction_Interaction
                                                      )
{
	CurrentWeather = WeatherSettings::Clear_Skies;

	CurrentHour = 12;
}

void FInteraction_Decorator::Entry()
{
	Super::Entry();
}

FSingleDeviceMode_Decorator::FSingleDeviceMode_Decorator(
	const TObjectPtr<AActor>& InTargetDevicePtr
	) :
	  Super(
	        USmartCitySuiteTags::Interaction_Mode,
	        USmartCitySuiteTags::Interaction_Mode_View
	       )
	  , TargetDevicePtr(InTargetDevicePtr)
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

		PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_SwitchSingleSceneElementState>(
			 false,
			 [this, FloorPtr, &AreaTag](
			 UGT_SwitchSingleSceneElementState* GTPtr
			 )
			 {
				 if (GTPtr)
				 {
					 GTPtr->
						 SceneElementFilter = FloorPtr->AllReference.SoftDecorationItem;

					 FSceneElementConditional FSceneElementConditional;

					 FSceneElementConditional.ConditionalSet.AddTag(AreaTag);

					 GTPtr->FilterTags = FSceneElementConditional;
				 }
			 }
			);
	}
}

void FInteraction_Decorator::SwitchIteractionType(
	EInteractionType NewInteractionType
	)
{
	InteractionType = NewInteractionType;
}

FInteraction_Decorator::EInteractionType FInteraction_Decorator::GetInteractionType() const
{
	return InteractionType;
}

FGameplayTag FInteraction_Decorator::GetCurrentWeather() const
{
	return CurrentWeather;
}

void FInteraction_Decorator::SetCurrentWeather(
	const FGameplayTag& WeatherTag
	)
{
	CurrentWeather = WeatherTag;

	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(CurrentWeather);
}

int32 FInteraction_Decorator::GetCurrentHour() const
{
	return CurrentHour;
}

void FInteraction_Decorator::SetCurrentHour(
	int32 Hour
	)
{
	CurrentHour = Hour;

	FDateTime Time(1, 1, 1, Hour);

	UWeatherSystem::GetInstance()->AdjustTime(Time);
}
