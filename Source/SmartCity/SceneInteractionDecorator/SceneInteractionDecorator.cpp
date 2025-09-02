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
#include "FloorHelper.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "SceneElement_PWR_Pipe.h"
#include "TemplateHelper.h"
#include "FloorHelperBase.h"
#include "NavagationPaths.h"
#include "SceneElement_Space.h"
#include "SmartCitySuiteTags.h"

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
	const TSet<AActor*>& InActors
	)
{
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
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);
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

FPWREnergyMode_Decorator::FPWREnergyMode_Decorator() :
                                                     Super(
                                                           USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_Energy
                                                          )
{
}

void FPWREnergyMode_Decorator::OnUpdateFilterComplete(
	bool bIsOK,
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);

	for (auto Iter : InActors)
	{
		auto PipePtr = Cast<ASceneElement_PWR_Pipe>(Iter);
		if (PipePtr)
		{
			PipeActors.Add(PipePtr);
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
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);
}

FSunShadeMode_Decorator::FSunShadeMode_Decorator() :
                                                   Super(
                                                         USmartCitySuiteTags::Interaction_Mode_DeviceManagger_PWR_SunShade
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
	const TSet<AActor*>& InActors
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
					const TSet<AActor*>&


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
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
					const TSet<AActor*>&


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
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
					const TSet<AActor*>&


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

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
		const TSet<AActor*>&


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
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
				const TSet<AActor*>&


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
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
				const TSet<AActor*>&


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
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
				const TSet<AActor*>&


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
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
				const TSet<AActor*>&


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
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
		const TSet<AActor*>&


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
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
	                                                                    [this](
	                                                                    UGT_FloorSplit* GTPtr
	                                                                    )
	                                                                    {
	                                                                    }
	                                                                   );
}

void FSplitFloor_Decorator::Quit()
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_QuitFloorSplit>(
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
					const TSet<AActor*>&


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
			else if (DecoratorSPtr->GetBranchDecoratorType().
			                        MatchesTag(USmartCitySuiteTags::Interaction_Mode_DeviceManagger_Elevator))
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
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
					const TSet<AActor*>&


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

				return;
			}
			else
			{
				FSceneElementConditional SceneActorConditional;

				SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
				SceneActorConditional.ConditionalSet.AddTag(DecoratorSPtr->GetBranchDecoratorType());

				TMulticastDelegate<void(
					bool,
					const TSet<AActor*>&


					
					)> MulticastDelegate;

				MulticastDelegate.AddRaw(DecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);
				MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

				USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
				                                                          SceneActorConditional,
				                                                          MulticastDelegate
				                                                         );

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
		const TSet<AActor*>&


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
	                                                          MulticastDelegate
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
				const TSet<AActor*>&


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
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

			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
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
				const TSet<AActor*>&


				
				)> MulticastDelegate;

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
			                                                          MulticastDelegate
			                                                         );
			return;
		}
		{
			FSceneElementConditional SceneActorConditional;

			SceneActorConditional.ConditionalSet.AddTag(GetBranchDecoratorType());
			SceneActorConditional.ConditionalSet.AddTag(NewDecoratorSPtr->GetBranchDecoratorType());


			TMulticastDelegate<void(
				bool,
				const TSet<AActor*>&


				
				)> MulticastDelegate;

			MulticastDelegate.AddRaw(NewDecoratorSPtr.Get(), &FDecoratorBase::OnUpdateFilterComplete);

			USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
			                                                          SceneActorConditional,
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
		const TSet<AActor*>&


		
		)> MulticastDelegate;

	MulticastDelegate.AddRaw(this, &ThisClass::OnUpdateFilterComplete);

	USceneInteractionWorldSystem::GetInstance()->UpdateFilter(
	                                                          SceneActorConditional,
	                                                          MulticastDelegate
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
				DynamicCastSharedPtr<FDeviceManaggerMode_Decorator>(
																	USceneInteractionWorldSystem::GetInstance()->
																	GetDecorator(
																		 USmartCitySuiteTags::Interaction_Mode
																		)
																   );
			if (DecoratorSPtr)
			{
			}
			else
			{
				
			}
			
			TArray<struct FHitResult> OutHits;

			auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));

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
				                                          WorldLocation + (WorldDirection * UGameOptions::GetInstance()
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

						USceneInteractionWorldSystem::GetInstance()->ClearFocus();

						FSceneElementConditional SceneActorConditional;

						SceneActorConditional.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode_View);

						USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(
							 Iter.GetActor(),
							 SceneActorConditional
							);

						return true;
					}
				}
			}

			// 检测区域
			{
				FCollisionObjectQueryParams ObjectQueryParams;
				ObjectQueryParams.AddObjectTypesToQuery(Space_Object);
				GetWorldImp()->LineTraceMultiByObjectType(
				                                          OutHits,
				                                          WorldLocation,
				                                          WorldLocation + (WorldDirection * UGameOptions::GetInstance()
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

						USceneInteractionWorldSystem::GetInstance()->ClearFocus();

						FSceneElementConditional SceneActorConditional;

						SceneActorConditional.ConditionalSet.AddTag(USmartCitySuiteTags::Interaction_Mode_Focus);

						USceneInteractionWorldSystem::GetInstance()->SwitchInteractionType(
							 Iter.GetActor(),
							 SceneActorConditional
							);

						PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransformLocaterBySpace>(
							 [&Iter](
							 UGT_CameraTransformLocaterBySpace* GTPtr
							 )
							 {
								 if (GTPtr)
								 {
									 GTPtr->SpaceActorPtr = Iter.GetActor();
								 }
							 }
							);

						return true;
					}
				}
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
	const TSet<AActor*>& InActors
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors);

	auto Result = UKismetAlgorithm::GetCameraSeat(
	                                              InActors,
	                                              UGameOptions::GetInstance()->ViewFloorControlParam.ViewRot,
	                                              UGameOptions::GetInstance()->ViewFloorControlParam.FOV
	                                             );

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_ModifyCameraTransform>(
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

FInteraction_Decorator::FInteraction_Decorator():
Super(
			USmartCitySuiteTags::Interaction_Interaction,
			USmartCitySuiteTags::Interaction_Interaction)
{
	
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
