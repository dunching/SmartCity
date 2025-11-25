#include "SceneInteractionDecorator_Area.h"

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
#include "Building_Floor.h"
#include "DatasmithSceneActor.h"
#include "Dynamic_WeatherBase.h"
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
#include "SceneElement_Computer.h"
#include "ViewSingleSpaceProcessor.h"
#include "ViewSpecialAreaProcessor.h"
#include "ViewerPawnBase.h"
#include "ViewPeripheryProcessor.h"

FArea_Decorator::FArea_Decorator(
	) :
	  Super(
	       )
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

void FArea_Decorator::InitialType(
	FGameplayTag InMainDecoratorType,
	FGameplayTag InBranchDecoratorType
	)
{
	Super::InitialType(InMainDecoratorType, InBranchDecoratorType);

	CurrentInteraction_Area = InBranchDecoratorType;
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
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
}

FViewTower_Decorator::FViewTower_Decorator(
	) :
	  Super(
	       )
{
}

void FViewTower_Decorator::Entry()
{
	Super::Entry();

	ON_SCOPE_EXIT
	{
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewTowerProcessor>(
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
						UGT_SwitchSceneElement_Base*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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
						UGT_SwitchSceneElement_Base*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
						 SceneActorConditional,
						 true,
						 MulticastDelegate
						);

					return;
				}
				else if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
				                                                            USmartCitySuiteTags::Interaction_Mode_View
				                                                           ))
				{
					USceneInteractionWorldSystem::GetInstance()->SwitchInteractionMode(FGameplayTag::EmptyTag);

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
						UGT_SwitchSceneElement_Base*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

						UGT_SwitchSceneElement_Base*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

		UGT_SwitchSceneElement_Base*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
	                                                                SceneActorConditional,
	                                                                true,
	                                                                MulticastDelegate
	                                                               );
}

void FViewTower_Decorator::OnOtherDecoratorEntry(
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
			                                                                SceneActorConditional,
			                                                                true,
			                                                                MulticastDelegate
			                                                               );
			return;
		}
		else if (NewDecoratorSPtr->GetBranchDecoratorType().
		                           MatchesTag(USmartCitySuiteTags::Interaction_Mode_View))
		{
			// USceneInteractionWorldSystem::GetInstance()->SwitchInteractionMode(FGameplayTag::EmptyTag);

			// return;
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

		UGT_SwitchSceneElement_Base*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
	                                                                SceneActorConditional,
	                                                                true,
	                                                                MulticastDelegate
	                                                               );
}

bool FViewTower_Decorator::Operation(
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

void FViewTower_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);
}

FViewPeriphery_Decorator::FViewPeriphery_Decorator() :
                                                     Super(
                                                          )
{
}

void FViewPeriphery_Decorator::Entry()
{
	Super::Entry();

	ON_SCOPE_EXIT
	{
		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewPeripheryProcessor>(
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

						UGT_SwitchSceneElement_Base*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

						UGT_SwitchSceneElement_Base*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
						 SceneActorConditional,
						 true,
						 MulticastDelegate
						);

					return;
				}
				else if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
				                                                            USmartCitySuiteTags::Interaction_Mode_View
				                                                           ))
				{
					USceneInteractionWorldSystem::GetInstance()->SwitchInteractionMode(FGameplayTag::EmptyTag);

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

						UGT_SwitchSceneElement_Base*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

						UGT_SwitchSceneElement_Base*


						
						)> MulticastDelegate;

					MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
					MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

					USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

		UGT_SwitchSceneElement_Base*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
	                                                                SceneActorConditional,
	                                                                true,
	                                                                MulticastDelegate
	                                                               );
}

void FViewPeriphery_Decorator::OnOtherDecoratorEntry(
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
			                                                                SceneActorConditional,
			                                                                true,
			                                                                MulticastDelegate
			                                                               );
			return;
		}
		else if (NewDecoratorSPtr->GetBranchDecoratorType().
		                           MatchesTag(USmartCitySuiteTags::Interaction_Mode_View))
		{
			// USceneInteractionWorldSystem::GetInstance()->SwitchInteractionMode(FGameplayTag::EmptyTag);

			// return;
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
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

		UGT_SwitchSceneElement_Base*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Tower(
	                                                                SceneActorConditional,
	                                                                true,
	                                                                MulticastDelegate
	                                                               );
}

bool FViewPeriphery_Decorator::Operation(
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

void FViewPeriphery_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);
}

FSplitFloor_Decorator::FSplitFloor_Decorator(
	) :
	  Super(
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
	) :
	  Super(
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
	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateCloudCoverageMunualOverride(true);
	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateCloudCoverage(0);

	FDateTime Time(1, 1, 1, 12);

	FViewConfig Config;

	bool bUseTemporaComfig = false;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this,&Config](
	                                                                  const TSharedPtr<FInteraction_Decorator>&
	                                                                  SPtr
	                                                                  )
	                                                                  {
		                                                                  Config = SPtr->GetViewConfig();

		                                                                  if (SPtr->HasViewConfigChanged())
		                                                                  {
		                                                                  }
		                                                                  else
		                                                                  {
			                                                                  Config.WallTranlucent = 30;
			                                                                  Config.PillarTranlucent = 30;
			                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  SPtr->UpdateViewConfig(Config, false);
		                                                                  }
	                                                                  },
	                                                                  false
	                                                                 );

	ON_SCOPE_EXIT
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this,&Config, &bUseTemporaComfig](
		                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
		                                                                  )
		                                                                  {
			                                                                  if (bUseTemporaComfig)
			                                                                  {
				                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  }
			                                                                  else
			                                                                  {
				                                                                  SPtr->ClearTemporaViewConfig();
			                                                                  }
		                                                                  },
		                                                                  false
		                                                                 );

		UWeatherSystem::GetInstance()->AdjustTime(Time);
		AdjustCamera();
	};

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

					UGT_SwitchSceneElement_Base*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
				                                                                SceneActorConditional,
				                                                                true,
				                                                                MulticastDelegate
				                                                               );

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

					UGT_SwitchSceneElement_Base*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
				                                                                SceneActorConditional,
				                                                                true,
				                                                                MulticastDelegate
				                                                               );

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
			                                                       USmartCitySuiteTags::Interaction_Mode_EmergencySystem
			                                                      ))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,

					UGT_SwitchSceneElement_Base*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
				                                                                SceneActorConditional,
				                                                                true,
				                                                                MulticastDelegate
				                                                               );

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

				bUseTemporaComfig = true;
				USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
					 USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
					 [this,&Config](
					 const TSharedPtr<FInteraction_Decorator>& SPtr
					 )
					 {
						 Config = SPtr->GetViewConfig();

						 Config.WallTranlucent = 100;
						 Config.PillarTranlucent = 100;
					 },
					 false
					);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
			                                                      ))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,

					UGT_SwitchSceneElement_Base*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
				                                                                SceneActorConditional,
				                                                                true,
				                                                                MulticastDelegate
				                                                               );

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

				Time = FDateTime(1, 1, UAssetRefMap::GetInstance()->ViewLightingTime);

				bUseTemporaComfig = true;
				USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
					 USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
					 [this,&Config](
					 const TSharedPtr<FInteraction_Decorator>& SPtr
					 )
					 {
						 Config = SPtr->GetViewConfig();

						 Config.WallTranlucent = 100;
						 Config.PillarTranlucent = 100;
					 },
					 false
					);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_EnergyManagement
			                                                      ))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,

					UGT_SwitchSceneElement_Base*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
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

					UGT_SwitchSceneElement_Base*


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
				                                                                SceneActorConditional,
				                                                                true,
				                                                                MulticastDelegate
				                                                               );

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
	}

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

	TMulticastDelegate<void(
		bool,

		UGT_SwitchSceneElement_Base*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
	                                                                SceneActorConditional,
	                                                                true,
	                                                                MulticastDelegate
	                                                               );

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

	if (Building_Floor_Mask)
	{
		Building_Floor_Mask->Destroy();
	}
	Building_Floor_Mask = nullptr;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this](
	                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
	                                                                  )
	                                                                  {
		                                                                  // SPtr->UpdateViewConfig(Config);
	                                                                  },
	                                                                  false
	                                                                 );
	Super::Quit();
}

void FFloor_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Interaction)
	)
	{
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
					if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
					                                                       USmartCitySuiteTags::Interaction_Mode_EmergencySystem
					                                                      ))
					{
						return;
					}
					if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
					                                                       USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
					                                                      ))
					{
						return;
					}
				}
			}
		}
	}

	bool bUseTemporaComfig = false;

	FDateTime Time(1, 1, 1, 12);

	FViewConfig Config;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this,&Config](
	                                                                  const TSharedPtr<FInteraction_Decorator>&
	                                                                  SPtr
	                                                                  )
	                                                                  {
		                                                                  Config = SPtr->GetViewConfig();

		                                                                  if (SPtr->HasViewConfigChanged())
		                                                                  {
		                                                                  }
		                                                                  else
		                                                                  {
			                                                                  Config.WallTranlucent = 30;
			                                                                  Config.PillarTranlucent = 30;
			                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  SPtr->UpdateViewConfig(Config, false);
		                                                                  }
	                                                                  },
	                                                                  false
	                                                                 );

	ON_SCOPE_EXIT
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this,&Config, &bUseTemporaComfig](
		                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
		                                                                  )
		                                                                  {
			                                                                  if (bUseTemporaComfig)
			                                                                  {
				                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  }
			                                                                  else
			                                                                  {
				                                                                  SPtr->ClearTemporaViewConfig();
			                                                                  }
		                                                                  },
		                                                                  false
		                                                                 );

		UWeatherSystem::GetInstance()->AdjustTime(Time);
	};

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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
			                                                                SceneActorConditional,
			                                                                true,
			                                                                MulticastDelegate
			                                                               );

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
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_EmergencySystem))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
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

			bUseTemporaComfig = true;
			USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
			                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
			                                                                  [this,&Config](
			                                                                  const TSharedPtr<FInteraction_Decorator>&
			                                                                  SPtr
			                                                                  )
			                                                                  {
				                                                                  Config = SPtr->GetViewConfig();

				                                                                  Config.WallTranlucent = 100;
				                                                                  Config.PillarTranlucent = 100;
			                                                                  },
			                                                                  false
			                                                                 );

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(
		                                                          USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
		                                                         ))
		{
			Time = FDateTime(1, 1, UAssetRefMap::GetInstance()->ViewLightingTime);

			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
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

			bUseTemporaComfig = true;
			USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
			                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
			                                                                  [this,&Config](
			                                                                  const TSharedPtr<FInteraction_Decorator>&
			                                                                  SPtr
			                                                                  )
			                                                                  {
				                                                                  Config = SPtr->GetViewConfig();
				                                                                  Config.WallTranlucent = 100;
				                                                                  Config.PillarTranlucent = 100;
			                                                                  },
			                                                                  false
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
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
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_View))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
			                                                                SceneActorConditional,
			                                                                true,
			                                                                MulticastDelegate
			                                                               );

			IncreaseWaitTaskCount();

			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
				TourProcessor::FViewSingleFloorProcessor>(
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
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar))
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			TMulticastDelegate<void(
				bool,

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
			                                                                SceneActorConditional,
			                                                                true,
			                                                                MulticastDelegate
			                                                               );

			IncreaseWaitTaskCount();

			UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
				TourProcessor::FViewSingleFloorProcessor>(
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

				UGT_SwitchSceneElement_Base*


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
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

			UGT_SwitchSceneElement_Base*


			
			)> MulticastDelegate;

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
		                                                                SceneActorConditional,
		                                                                true,
		                                                                MulticastDelegate
		                                                               );

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

		UGT_SwitchSceneElement_Base*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
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
				case EInteractionType::kDevice:
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

									auto SpaceElementPtr = Cast<ASceneElement_DeviceBase>(Iter.GetActor());
									if (!SpaceElementPtr)
									{
										continue;
									}

									USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
										 USmartCitySuiteTags::Interaction_Area_ViewDevice,
										 [this, SpaceElementPtr](
										 const TSharedPtr<FDecoratorBase>& AreaDecoratorSPtr
										 )
										 {
											 auto SpaceAreaDecoratorSPtr = DynamicCastSharedPtr<FViewDevice_Decorator>(
												  AreaDecoratorSPtr
												 );
											 if (SpaceAreaDecoratorSPtr)
											 {
												 SpaceAreaDecoratorSPtr->SceneElementPtr = SpaceElementPtr;
											 }
										 }
										);

									return true;
								}
							}
						}
					}
					break;
				case EInteractionType::kSpace:
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

							FCollisionQueryParams Params;

							Params.bTraceComplex = true;

							GetWorldImp()->LineTraceMultiByObjectType(
							                                          OutHits,
							                                          WorldLocation,
							                                          WorldLocation + (
								                                          WorldDirection * UGameOptions::GetInstance()
								                                          ->LinetraceDistance),
							                                          ObjectQueryParams,
							                                          Params
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

									USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
										 USmartCitySuiteTags::Interaction_Area_Space,
										 [this, SpaceElementPtr](
										 const TSharedPtr<FDecoratorBase>& AreaDecoratorSPtr
										 )
										 {
											 auto SpaceAreaDecoratorSPtr = DynamicCastSharedPtr<FViewSpace_Decorator>(
												  AreaDecoratorSPtr
												 );
											 if (SpaceAreaDecoratorSPtr)
											 {
												 SpaceAreaDecoratorSPtr->Floor = SpaceElementPtr->BelongFloor->FloorTag;
												 SpaceAreaDecoratorSPtr->SceneElementPtr = SpaceElementPtr;
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

void FFloor_Decorator::UpdateParam(
	const TMap<FString, TMap<FString, FString>>& ExtensionParamMap
	)
{
	
}

void FFloor_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);

	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->FloorTag.MatchesTag(GetBranchDecoratorType()))
		{
			if (Building_Floor_Mask)
			{
			}
			else
			{
				Building_Floor_Mask = GetWorldImp()->SpawnActor<ABuilding_Floor_Mask>(
					 UAssetRefMap::GetInstance()->Building_Floor_MaskClass
					);
			}
			if (Building_Floor_Mask)
			{
				Building_Floor_Mask->SetFloor(FloorIter.Value.LoadSynchronous());
			}

			break;
		}
	}

	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->GameplayTagContainer.HasTag(GetBranchDecoratorType()))
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
						MessageSPtr->SpacesMap.Add(SpacePtr, SpacePtr->GetAllDevices());
					}
				}
			}

			MessageSPtr->FloorHelper = Cast<AFloorHelper>(FloorIter.Value.LoadSynchronous());
			MessageSPtr->PresetBuildingCameraSeat = FloorIter.Value->GetPresetBuildingCameraSeat();

			UWebChannelWorldSystem::GetInstance()->SendMessage(MessageSPtr);
		}
	}
}

void FFloor_Decorator::Process()
{
}

void FFloor_Decorator::AdjustCamera() const
{
	for (const auto& FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
	{
		if (FloorIter.Value->FloorTag.MatchesTag(GetBranchDecoratorType()))
		{
			if (FloorIter.Value->DefaultBuildingCameraSeat.IsValid())
			{
				auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
				PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransformByPawnViewer>(
					 false,
					 [FloorIter](
					 UGT_CameraTransformByPawnViewer* GTPtr
					 )
					 {
						 if (GTPtr)
						 {
							 GTPtr->
								 ViewerPawnPtr
								 = FloorIter.Value->DefaultBuildingCameraSeat.LoadSynchronous();
						 }
					 }
					);
			}
			else
			{
				auto Result = FloorIter.Value->GetCameraSeat(
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
		}
	}
}

FViewDevice_Decorator::FViewDevice_Decorator(
	) :
	  Super(
	       )
{
}

void FViewDevice_Decorator::Entry()
{
	Super::Entry();

	Process();
}

void FViewDevice_Decorator::ReEntry()
{
	Super::ReEntry();

	Process();
}

void FViewDevice_Decorator::Quit()
{
	if (Building_Floor_Mask)
	{
		Building_Floor_Mask->Destroy();
	}
	Building_Floor_Mask = nullptr;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this](
	                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
	                                                                  )
	                                                                  {
		                                                                  SPtr->ClearTemporaViewConfig();
	                                                                  },
	                                                                  false
	                                                                 );
	Super::Quit();
}

void FViewDevice_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Interaction)
	)
	{
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
					if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
					                                                       USmartCitySuiteTags::Interaction_Mode_EmergencySystem
					                                                      ))
					{
						return;
					}
					if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
					                                                       USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
					                                                      ))
					{
						return;
					}
				}
			}
		}
	}

	bool bUseTemporaComfig = false;

	FDateTime Time(1, 1, 1, 12);

	FViewConfig Config;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this,&Config](
	                                                                  const TSharedPtr<FInteraction_Decorator>&
	                                                                  SPtr
	                                                                  )
	                                                                  {
		                                                                  Config = SPtr->GetViewConfig();

		                                                                  if (SPtr->HasViewConfigChanged())
		                                                                  {
		                                                                  }
		                                                                  else
		                                                                  {
			                                                                  Config.WallTranlucent = 30;
			                                                                  Config.PillarTranlucent = 30;
			                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  SPtr->UpdateViewConfig(Config, false);
		                                                                  }
	                                                                  },
	                                                                  false
	                                                                 );

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(SceneElementPtr->BelongFloor->FloorTag);

	TMulticastDelegate<void(
		bool,

		UGT_SwitchSceneElement_Base*
		
		)> MulticastDelegate;

	ON_SCOPE_EXIT
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this,&Config, &bUseTemporaComfig](
		                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
		                                                                  )
		                                                                  {
			                                                                  if (bUseTemporaComfig)
			                                                                  {
				                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  }
			                                                                  else
			                                                                  {
				                                                                  SPtr->ClearTemporaViewConfig();
			                                                                  }
		                                                                  },
		                                                                  false
		                                                                 );

		UWeatherSystem::GetInstance()->AdjustTime(Time);

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Device(
		                                                                 SceneActorConditional,
		                                                                 true,
		                                                                 MulticastDelegate,
		                                                                 SceneElementPtr
		                                                                );

		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
			TourProcessor::FViewSingleDeviceProcessor>(
			                                           [this](
			                                           auto NewProcessor
			                                           )
			                                           {
				                                           NewProcessor->SceneElementPtr = SceneElementPtr;
			                                           }
			                                          );
	};

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode)
	)
	{
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode_Empty))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(
		                                                          USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator
		                                                         ))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_EmergencySystem))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			bUseTemporaComfig = true;
			USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
			                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
			                                                                  [this,&Config](
			                                                                  const TSharedPtr<FInteraction_Decorator>&
			                                                                  SPtr
			                                                                  )
			                                                                  {
				                                                                  Config = SPtr->GetViewConfig();

				                                                                  Config.WallTranlucent = 100;
				                                                                  Config.PillarTranlucent = 100;
			                                                                  },
			                                                                  false
			                                                                 );

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(
		                                                          USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
		                                                         ))
		{
			Time = FDateTime(1, 1, UAssetRefMap::GetInstance()->ViewLightingTime);

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
			                                                                SceneActorConditional,
			                                                                true,
			                                                                MulticastDelegate
			                                                               );

			IncreaseWaitTaskCount();

			bUseTemporaComfig = true;
			USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
			                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
			                                                                  [this,&Config](
			                                                                  const TSharedPtr<FInteraction_Decorator>&
			                                                                  SPtr
			                                                                  )
			                                                                  {
				                                                                  Config = SPtr->GetViewConfig();
				                                                                  Config.WallTranlucent = 100;
				                                                                  Config.PillarTranlucent = 100;
			                                                                  },
			                                                                  false
			                                                                 );

			return;
		}

		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_EnergyManagement))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_View))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			return;
		}

		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			return;
		}
	}

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Interaction)
	)
	{
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

		return;
	}

	SceneActorConditional.ConditionalSet = USceneInteractionWorldSystem::GetInstance()->GetAllInteractionTags();

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
}

void FViewDevice_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);

	if (Building_Floor_Mask)
	{
	}
	else
	{
		Building_Floor_Mask = GetWorldImp()->SpawnActor<ABuilding_Floor_Mask>(
		                                                                      UAssetRefMap::GetInstance()->
		                                                                      Building_Floor_MaskClass
		                                                                     );
	}
	if (Building_Floor_Mask)
	{
		Building_Floor_Mask->SetFloor(SceneElementPtr->BelongFloor);
	}

	auto MessageSPtr = MakeShared<FMessageBody_SelectedDevice>();

	MessageSPtr->DeviceIDAry.Add(SceneElementPtr->SceneElementID);

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageSPtr);
}

void FViewDevice_Decorator::Process()
{
	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(WeatherSettings::Clear_Skies);
	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateCloudCoverageMunualOverride(true);
	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateCloudCoverage(0);

	FDateTime Time(1, 1, 1, 12);

	FViewConfig Config;

	bool bUseTemporaComfig = false;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this,&Config](
	                                                                  const TSharedPtr<FInteraction_Decorator>&
	                                                                  SPtr
	                                                                  )
	                                                                  {
		                                                                  Config = SPtr->GetViewConfig();

		                                                                  if (SPtr->HasViewConfigChanged())
		                                                                  {
		                                                                  }
		                                                                  else
		                                                                  {
			                                                                  Config.WallTranlucent = 10;
			                                                                  Config.PillarTranlucent = 10;
			                                                                  Config.StairsTranlucent = 10;
			                                                                  Config.CurtainWallTranlucent = 10;
			                                                                  Config.FurnitureTranlucent = 10;
			                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  SPtr->UpdateViewConfig(Config, false);
		                                                                  }
	                                                                  },
	                                                                  false
	                                                                 );

	FSceneElementConditional SceneActorConditional;

	SceneActorConditional.ConditionalSet.AddTag(SceneElementPtr->BelongFloor->FloorTag);

	TMulticastDelegate<void(
		bool,

		UGT_SwitchSceneElement_Base*
		
		)> MulticastDelegate;

	ON_SCOPE_EXIT
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this,&Config, &bUseTemporaComfig](
		                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
		                                                                  )
		                                                                  {
			                                                                  if (bUseTemporaComfig)
			                                                                  {
				                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  }
			                                                                  else
			                                                                  {
				                                                                  SPtr->ClearTemporaViewConfig();
			                                                                  }
		                                                                  },
		                                                                  false
		                                                                 );

		UWeatherSystem::GetInstance()->AdjustTime(Time);

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Device(
		                                                                 SceneActorConditional,
		                                                                 true,
		                                                                 MulticastDelegate,
		                                                                 SceneElementPtr
		                                                                );

		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
			TourProcessor::FViewSingleDeviceProcessor>(
			                                           [this](
			                                           auto NewProcessor
			                                           )
			                                           {
				                                           NewProcessor->SceneElementPtr = SceneElementPtr;
			                                           }
			                                          );

		AdjustCamera();
	};

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
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().
			                   MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator))
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_EmergencySystem
			                                                      ))
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				bUseTemporaComfig = true;
				USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
					 USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
					 [this,&Config](
					 const TSharedPtr<FInteraction_Decorator>& SPtr
					 )
					 {
						 Config = SPtr->GetViewConfig();

						 Config.WallTranlucent = 100;
						 Config.PillarTranlucent = 100;
					 },
					 false
					);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
			                                                      ))
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				Time = FDateTime(1, 1, UAssetRefMap::GetInstance()->ViewLightingTime);

				bUseTemporaComfig = true;
				USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
					 USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
					 [this,&Config](
					 const TSharedPtr<FInteraction_Decorator>& SPtr
					 )
					 {
						 Config = SPtr->GetViewConfig();

						 Config.WallTranlucent = 100;
						 Config.PillarTranlucent = 100;
					 },
					 false
					);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_EnergyManagement
			                                                      ))
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				return;
			}
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				return;
			}
		}
	}

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
}

void FViewDevice_Decorator::AdjustCamera() const
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransformLocaterByID>(
		 false,
		 [this](
		 UGT_CameraTransformLocaterByID* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->TargetDevicePtr = SceneElementPtr.Get();
			 }
		 }
		);
}

FViewSpace_Decorator::FViewSpace_Decorator() :
                                             Super(
                                                  )
{
}

void FViewSpace_Decorator::Entry()
{
	Super::Entry();

	Process();
}

void FViewSpace_Decorator::ReEntry()
{
	Super::ReEntry();

	Process();
}

void FViewSpace_Decorator::Quit()
{
	if (Building_Floor_Mask)
	{
		Building_Floor_Mask->Destroy();
	}
	Building_Floor_Mask = nullptr;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this](
	                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
	                                                                  )
	                                                                  {
		                                                                  SPtr->ClearTemporaViewConfig();
	                                                                  },
	                                                                  false
	                                                                 );
	Super::Quit();
}

void FViewSpace_Decorator::OnOtherDecoratorEntry(
	const TSharedPtr<FDecoratorBase>& NewDecoratorSPtr
	)
{
	Super::OnOtherDecoratorEntry(NewDecoratorSPtr);

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Interaction)
	)
	{
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
					if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
					                                                       USmartCitySuiteTags::Interaction_Mode_EmergencySystem
					                                                      ))
					{
						return;
					}
					if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
					                                                       USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
					                                                      ))
					{
						return;
					}
				}
			}
		}
	}

	bool bUseTemporaComfig = false;

	FDateTime Time(1, 1, 1, 12);

	FViewConfig Config;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this,&Config](
	                                                                  const TSharedPtr<FInteraction_Decorator>&
	                                                                  SPtr
	                                                                  )
	                                                                  {
		                                                                  Config = SPtr->GetViewConfig();

		                                                                  if (SPtr->HasViewConfigChanged())
		                                                                  {
		                                                                  }
		                                                                  else
		                                                                  {
			                                                                  Config.WallTranlucent = 30;
			                                                                  Config.PillarTranlucent = 30;
			                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  SPtr->UpdateViewConfig(Config, false);
		                                                                  }
	                                                                  },
	                                                                  false
	                                                                 );

	FSceneElementConditional SceneActorConditional;

	TMulticastDelegate<void(
		bool,

		UGT_SwitchSceneElement_Base*
		
		)> MulticastDelegate;

	ON_SCOPE_EXIT
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this,&Config, &bUseTemporaComfig](
		                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
		                                                                  )
		                                                                  {
			                                                                  if (bUseTemporaComfig)
			                                                                  {
				                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  }
			                                                                  else
			                                                                  {
				                                                                  SPtr->ClearTemporaViewConfig();
			                                                                  }
		                                                                  },
		                                                                  false
		                                                                 );

		UWeatherSystem::GetInstance()->AdjustTime(Time);

		USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Space(
		                                                                SceneActorConditional,
		                                                                true,
		                                                                MulticastDelegate,
		                                                                SceneElementPtr
		                                                               );

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
	};

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode)
	)
	{
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Mode_Empty))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(
		                                                          USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator
		                                                         ))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_EmergencySystem))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			bUseTemporaComfig = true;
			USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
			                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
			                                                                  [this,&Config](
			                                                                  const TSharedPtr<FInteraction_Decorator>&
			                                                                  SPtr
			                                                                  )
			                                                                  {
				                                                                  Config = SPtr->GetViewConfig();

				                                                                  Config.WallTranlucent = 100;
				                                                                  Config.PillarTranlucent = 100;
			                                                                  },
			                                                                  false
			                                                                 );

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().MatchesTag(
		                                                          USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
		                                                         ))
		{
			Time = FDateTime(1, 1, UAssetRefMap::GetInstance()->ViewLightingTime);

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Floor(
			                                                                SceneActorConditional,
			                                                                true,
			                                                                MulticastDelegate
			                                                               );

			IncreaseWaitTaskCount();

			bUseTemporaComfig = true;
			USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
			                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
			                                                                  [this,&Config](
			                                                                  const TSharedPtr<FInteraction_Decorator>&
			                                                                  SPtr
			                                                                  )
			                                                                  {
				                                                                  Config = SPtr->GetViewConfig();
				                                                                  Config.WallTranlucent = 100;
				                                                                  Config.PillarTranlucent = 100;
			                                                                  },
			                                                                  false
			                                                                 );

			return;
		}

		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_EnergyManagement))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_View))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			return;
		}
		if (NewDecoratorSPtr->GetBranchDecoratorType().
		                      MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_ELV_Radar))
		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			return;
		}

		{
			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			return;
		}
	}

	if (
		NewDecoratorSPtr->GetMainDecoratorType().MatchesTag(USmartCitySuiteTags::Interaction_Interaction)
	)
	{
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

		return;
	}

	SceneActorConditional.ConditionalSet = USceneInteractionWorldSystem::GetInstance()->GetAllInteractionTags();

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
}

void FViewSpace_Decorator::UpdateParam(
	const TMap<FString, TMap<FString, FString>>& ExtensionParamMap
	)
{
}

void FViewSpace_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);

	if (Building_Floor_Mask)
	{
	}
	else
	{
		Building_Floor_Mask = GetWorldImp()->SpawnActor<ABuilding_Floor_Mask>(
		                                                                      UAssetRefMap::GetInstance()->
		                                                                      Building_Floor_MaskClass
		                                                                     );
	}
	if (Building_Floor_Mask)
	{
		Building_Floor_Mask->SetFloor(SceneElementPtr->BelongFloor);
	}

	auto MessageSPtr = MakeShared<FMessageBody_SelectedSpace>();

	for (auto Iter : SceneElementPtr->GetAllDevices())
	{
		FMessageBody_SelectedSpace::FDeviceInfo DeviceInfo;

		DeviceInfo.DeviceID = Iter->SceneElementID;
		DeviceInfo.Type = Iter->DeviceTypeStr;

		MessageSPtr->DeviceIDAry.Add(DeviceInfo);
	}

	MessageSPtr->SpaceName = SceneElementPtr->Category;

	UWebChannelWorldSystem::GetInstance()->SendMessage(MessageSPtr);
}

void FViewSpace_Decorator::Process()
{
	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateWeather(WeatherSettings::Clear_Skies);
	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateCloudCoverageMunualOverride(true);
	UWeatherSystem::GetInstance()->GetDynamicWeather()->UpdateCloudCoverage(0);

	FDateTime Time(1, 1, 1, 12);

	FViewConfig Config;

	bool bUseTemporaComfig = false;

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this,&Config](
	                                                                  const TSharedPtr<FInteraction_Decorator>&
	                                                                  SPtr
	                                                                  )
	                                                                  {
		                                                                  Config = SPtr->GetViewConfig();

		                                                                  if (SPtr->HasViewConfigChanged())
		                                                                  {
		                                                                  }
		                                                                  else
		                                                                  {
			                                                                  Config.WallTranlucent = 10;
			                                                                  Config.PillarTranlucent = 10;
			                                                                  Config.StairsTranlucent = 10;
			                                                                  Config.CurtainWallTranlucent = 10;
			                                                                  Config.FurnitureTranlucent = 10;
			                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  SPtr->UpdateViewConfig(Config, false);
		                                                                  }
	                                                                  },
	                                                                  false
	                                                                 );

	FSceneElementConditional SceneActorConditional;

	TMulticastDelegate<void(
		bool,

		UGT_SwitchSceneElement_Base*
		
		)> MulticastDelegate;

	ON_SCOPE_EXIT
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this,&Config, &bUseTemporaComfig](
		                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
		                                                                  )
		                                                                  {
			                                                                  if (bUseTemporaComfig)
			                                                                  {
				                                                                  SPtr->UpdateViewConfig(Config, true);
			                                                                  }
			                                                                  else
			                                                                  {
				                                                                  SPtr->ClearTemporaViewConfig();
			                                                                  }
		                                                                  },
		                                                                  false
		                                                                 );

		UWeatherSystem::GetInstance()->AdjustTime(Time);

		SceneActorConditional.ConditionalSet.AddTag(Floor);
		USceneInteractionWorldSystem::GetInstance()->UpdateFilter_Space(
		                                                                SceneActorConditional,
		                                                                true,
		                                                                MulticastDelegate,
		                                                                SceneElementPtr
		                                                               );

		UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<
			TourProcessor::FViewSingleSpaceProcessor>(
			                                          [this](
			                                          auto NewProcessor
			                                          )
			                                          {
				                                          NewProcessor->SceneElementPtr = SceneElementPtr;
			                                          }
			                                         );

		AdjustCamera();
	};

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
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().
			                   MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator))
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_EmergencySystem
			                                                      ))
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				bUseTemporaComfig = true;
				USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
					 USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
					 [this,&Config](
					 const TSharedPtr<FInteraction_Decorator>& SPtr
					 )
					 {
						 Config = SPtr->GetViewConfig();

						 Config.WallTranlucent = 100;
						 Config.PillarTranlucent = 100;
					 },
					 false
					);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Lighting
			                                                      ))
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				Time = FDateTime(1, 1, UAssetRefMap::GetInstance()->ViewLightingTime);

				bUseTemporaComfig = true;
				USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
					 USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
					 [this,&Config](
					 const TSharedPtr<FInteraction_Decorator>& SPtr
					 )
					 {
						 Config = SPtr->GetViewConfig();

						 Config.WallTranlucent = 100;
						 Config.PillarTranlucent = 100;
					 },
					 false
					);

				return;
			}
			if (DecoratorSPtr->GetBranchDecoratorType().MatchesTag(
			                                                       USmartCitySuiteTags::Interaction_Mode_EnergyManagement
			                                                      ))
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				return;
			}
			{
				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				return;
			}
		}
	}

	SceneActorConditional.ConditionalSet.AddTag(Floor);
	SceneActorConditional.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Area_Space);
	SceneActorConditional.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode_View);

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);
}

void FViewSpace_Decorator::AdjustCamera() const
{
	auto PCPtr = Cast<APlanetPlayerController>(
	                                           GEngine->GetFirstLocalPlayerController(GetWorldImp())
	                                          );

	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<
		UGT_CameraTransformLocaterBySpace>(
		                                   false,
		                                   [this](
		                                   UGT_CameraTransformLocaterBySpace* GTPtr
		                                   )
		                                   {
			                                   if (GTPtr)
			                                   {
				                                   GTPtr->SpaceActorPtr = SceneElementPtr;
			                                   }
		                                   }
		                                  );
}

void FViewSpecialArea_Decorator::Entry()
{
	Super::Entry();

	UInputProcessorSubSystem_Imp::GetInstance()->SwitchToProcessor<TourProcessor::FViewSpecialAreaProcessor>(
		 [](
		 auto NewProcessor
		 )
		 {
		 }
		);

	auto DecoratorSPtr =
		DynamicCastSharedPtr<FInteraction_Decorator>(
		                                             USceneInteractionWorldSystem::GetInstance()->
		                                             GetDecorator(
		                                                          USmartCitySuiteTags::Interaction_Interaction
		                                                         )
		                                            );
	if (DecoratorSPtr)
	{
		USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
		                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
		                                                                  [this](
		                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
		                                                                  )
		                                                                  {
			                                                                  auto TempConfig = SPtr->GetViewConfig();
			                                                                  TempConfig.WallTranlucent = 10;
			                                                                  TempConfig.PillarTranlucent = 10;
			                                                                  TempConfig.StairsTranlucent = 10;
			                                                                  TempConfig.CurtainWallTranlucent = 10;
			                                                                  TempConfig.FurnitureTranlucent = 10;

			                                                                  SPtr->UpdateViewConfig(TempConfig, true);
		                                                                  },
		                                                                  false
		                                                                 );
	}

	Process();
}

void FViewSpecialArea_Decorator::ReEntry()
{
	Super::ReEntry();

	Process();
}

void FViewSpecialArea_Decorator::Quit()
{
	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [this](
	                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
	                                                                  )
	                                                                  {
		                                                                  SPtr->ClearTemporaViewConfig();
	                                                                  },
	                                                                  false
	                                                                 );
	Super::Quit();
}

void FViewSpecialArea_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, TaskPtr);
}

void FViewSpecialArea_Decorator::Process()
{
	FSceneElementConditional SceneActorConditional;

	for (auto Iter : FloorSet)
	{
		SceneActorConditional.ConditionalSet.AddTag(Iter);
	}

	TMulticastDelegate<void(
		bool,

		UGT_SwitchSceneElement_Base*


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter_SpeacialArea(
	                                                                       SceneActorConditional,
	                                                                       true,
	                                                                       MulticastDelegate,
	                                                                       FloorSet,
	                                                                       PriorityHideFloorSet
	                                                                      );

	AdjustCamera();
}

void FViewSpecialArea_Decorator::AdjustCamera() const
{
	auto PCPtr = Cast<APlanetPlayerController>(
	                                           GEngine->GetFirstLocalPlayerController(GetWorldImp())
	                                          );

	if (ViewerPawnBasePtr.IsValid())
	{
		PCPtr->GameplayTasksComponentPtr->StartGameplayTask<
			UGT_CameraTransformByPawnViewer>(
			                                 false,
			                                 [this](
			                                 UGT_CameraTransformByPawnViewer* GTPtr
			                                 )
			                                 {
				                                 if (GTPtr)
				                                 {
					                                 GTPtr->ViewerPawnPtr = ViewerPawnBasePtr.Get();
				                                 }
			                                 }
			                                );
	}
	else
	{
		for (auto FloorIter : UAssetRefMap::GetInstance()->FloorHelpers)
		{
			if (
				FloorIter.Key.MatchesTag(USmartCitySuiteTags::Interaction_Area_Floor_F12JF) &&
				FloorIter.Key.MatchesTag(AreaTag)
			)
			{
				for (auto Iter : FloorIter.Value->SceneElementCategoryMap)
				{
					TArray<AActor*> OutActors;

					Iter.Value->GetAttachedActors(OutActors, true, true);

					for (auto ActorIter : OutActors)
					{
						auto SceneElementBasePtr = Cast<ASceneElement_Computer>(ActorIter);
						if (SceneElementBasePtr && SceneElementBasePtr->DeviceTypeStr == Seat)
						{
							const auto ViewSeat = SceneElementBasePtr->GetViewSeat();
							PCPtr->GameplayTasksComponentPtr->StartGameplayTask<
								UGT_CameraTransform>(
								                     false,
								                     [this, ViewSeat](
								                     UGT_CameraTransform* GTPtr
								                     )
								                     {
									                     if (GTPtr)
									                     {
										                     GTPtr->TargetLocation = ViewSeat.Key.GetLocation();
										                     GTPtr->TargetRotation = ViewSeat.Key.GetRotation().
											                     Rotator();
										                     GTPtr->TargetTargetArmLength = ViewSeat.Value;
									                     }
								                     }
								                    );

							return;
						}
					}
				}
			}
		}
	}
}
