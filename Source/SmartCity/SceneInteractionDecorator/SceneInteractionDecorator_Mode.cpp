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
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElement_Base* TaskPtr
	)
{
	Super::OnUpdateFilterComplete(bIsOK, InActors, TaskPtr);
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
			auto FireMarkClass = UAssetRefMap::GetInstance()->FireMarkClass;

			const auto Box = Iter.Value->BoxComponentPtr->GetLocalBounds();
			const auto Location = Iter.Value->BoxComponentPtr->GetComponentLocation();
			auto Center = Box.GetBox().GetCenter();
			auto Extent = Box.GetBox().GetExtent();
			Extent.Z = 0;

			for (int32 Index = 0; Index < 3; Index++)
			{
				auto Pt = UKismetMathLibrary::RandomPointInBoundingBox(Location, Extent);

				auto FireMarkPtr = GetWorldImp()->SpawnActor<AFireMark>(FireMarkClass, Pt, FRotator::ZeroRotator);
				FireMarkSet.Add(FireMarkPtr);
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
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElement_Base* TaskPtr
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
	const TSet<AActor*>& InActors,
	UGT_SwitchSceneElement_Base* TaskPtr
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
