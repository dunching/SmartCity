#include "GameplayCommand.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "ElevatorSubSystem.h"
#include "GameplayTagsLibrary.h"
#include "IPSSI.h"
#include "LogWriter.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "Tools.h"
#include "SceneInteractionWorldSystem.h"
#include "ViewTowerProcessor.h"
#include "TourPawn.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "SceneElement_Space.h"
#include "SceneInteractionDecorator_Area.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"
#include "Kismet/KismetStringLibrary.h"

void SmartCityCommand::ReplyCameraTransform()
{
	USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
	                                                                   USmartCitySuiteTags::Interaction_Area_ExternalWall
	                                                                   .GetTag()
	                                                                  );
}

void SmartCityCommand::AdjustCameraSeat(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	UWebChannelWorldSystem::GetInstance()->OnInput(
												   FString::Printf(
																   TEXT(
																		R"({
    "CMD": "AdjustCameraSeat",
    "MinPitch": %s,
    "MaxPitch": %s
})"
																	   ),
																   *Args[0],
																   *Args[0]
																  )
												  );
}

void SmartCityCommand::SwitchInteractionType(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	UWebChannelWorldSystem::GetInstance()->OnInput(
												   FString::Printf(
																   TEXT(
																		R"({
    "CMD": "UpdateInteractionType",
    "InteractionType": "%s"
})"
																	   ),
																   *Args[0]
																  )
												  );
}

void SmartCityCommand::SwitchViewArea(
	const TArray<FString>& Args
	)
{
	for (auto Iter : Args)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(FGameplayTag::RequestGameplayTag(*Iter));
		return;
	}
}

void SmartCityCommand::SwitchMode(
	const TArray<FString>& Args
	)
{
	for (auto Iter : Args)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchInteractionMode(FGameplayTag::RequestGameplayTag(*Iter));
		return;
	}
}

void SmartCityCommand::SwitchInteraction(
	const TArray<FString>& Args
	)
{
	for (auto Iter : Args)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchInteractionOption(FGameplayTag::RequestGameplayTag(*Iter));
		return;
	}
}

void SmartCityCommand::TestAssetUserData()
{
	TArray<AActor*> ResultAry;
	UGameplayStatics::GetAllActorsOfClass(
	                                      GetWorldImp(),
	                                      AStaticMeshActor::StaticClass(),
	                                      ResultAry
	                                     );

	for (auto Iter : ResultAry)
	{
		auto STMPtr = Cast<AStaticMeshActor>(Iter);
		if (STMPtr)
		{
			auto AUDPtr = Cast<UDatasmithAssetUserData>(
			                                            STMPtr->GetStaticMeshComponent()->GetAssetUserDataOfClass(
				                                             UDatasmithAssetUserData::StaticClass()
				                                            )
			                                           );
			if (AUDPtr)
			{
				for (const auto& SecondIter : AUDPtr->MetaData)
				{
					PRINTINVOKEWITHSTR(FString::Printf(TEXT("%s %s"), *SecondIter.Key.ToString(), *SecondIter.Value));
				}
			}
		}
	}
}

void SmartCityCommand::AddFeatureItem(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
	if (HUDPtr)
	{
		// HUDPtr->GetMainHUDLayout()->InitalFeaturesItem(Args[0], Args);
	}
}

void SmartCityCommand::LocaterByID(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	auto DevicePtr = Cast<ASceneElement_DeviceBase>(
	                                                USceneInteractionWorldSystem::GetInstance()->FindSceneActor(Args[0])
	                                               );
	if (DevicePtr)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchInteractionArea(
		                                                                   USmartCitySuiteTags::Interaction_Area_ViewDevice,
		                                                                   [DevicePtr](
		                                                                   const TSharedPtr<FDecoratorBase>&
		                                                                   AreaDecoratorSPtr
		                                                                   )
		                                                                   {
			                                                                   auto SpaceAreaDecoratorSPtr =
				                                                                   DynamicCastSharedPtr<
					                                                                   FViewDevice_Decorator>(
					                                                                    AreaDecoratorSPtr
					                                                                   );
			                                                                   if (SpaceAreaDecoratorSPtr)
			                                                                   {
				                                                                   SpaceAreaDecoratorSPtr->
					                                                                   SceneElementPtr = DevicePtr;
			                                                                   }
		                                                                   }
		                                                                  );
	}
}

void SmartCityCommand::ElevatorMoveToFloor(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	UElevatorSubSystem::GetInstance()->ElevatorMoveToFloor(
	                                                       FGameplayTag::RequestGameplayTag(*Args[0]),
	                                                       UKismetStringLibrary::Conv_StringToInt(Args[1])
	                                                      );
}

void SmartCityCommand::SetSpaceFeature(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(
	                                      GetWorldImp(),
	                                      ASceneElement_Space::StaticClass(),
	                                      OutActors
	                                     );

	for (auto ActorIter : OutActors)
	{
		auto SpacePtr = Cast<ASceneElement_Space>(ActorIter);
		if (SpacePtr && SpacePtr->Category == Args[0])
		{
			auto Ary = Args;
			Ary.RemoveAt(0);
			SpacePtr->SetFeatures(Ary);

			return;
		}
	}
}

void SmartCityCommand::SetWallTranlucent(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [&Args](
	                                                                  const TSharedPtr<FInteraction_Decorator>&
	                                                                  DecoratorSPtr
	                                                                  )
	                                                                  {
		                                                                  auto ViewConfig = DecoratorSPtr->
			                                                                  GetViewConfig();
		                                                                  ViewConfig.WallTranlucent =
			                                                                  UKismetStringLibrary::Conv_StringToInt(
				                                                                   Args[0]
				                                                                  );
		                                                                  DecoratorSPtr->UpdateViewConfig(ViewConfig, false);
	                                                                  },
	                                                                  true
	                                                                 );
}

void SmartCityCommand::SetPillarTranlucent(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [&Args](
	                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
	                                                                  )
	                                                                  {
		                                                                  auto ViewConfig = SPtr->
			                                                                  GetViewConfig();
		                                                                  ViewConfig.PillarTranlucent =
			                                                                  UKismetStringLibrary::Conv_StringToInt(
				                                                                   Args[0]
				                                                                  );
		                                                                  SPtr->UpdateViewConfig(ViewConfig, false);
	                                                                  },
	                                                                  true
	                                                                 );
}

void SmartCityCommand::SetStairsTranlucent(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [&Args](
	                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
	                                                                  )
	                                                                  {
		                                                                  auto ViewConfig = SPtr->
			                                                                  GetViewConfig();
		                                                                  ViewConfig.StairsTranlucent =
			                                                                  UKismetStringLibrary::Conv_StringToInt(
				                                                                   Args[0]
				                                                                  );
		                                                                  SPtr->UpdateViewConfig(ViewConfig, false);
	                                                                  },
	                                                                  true
	                                                                 );
}

void SmartCityCommand::SetShowCurtainWall(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [&Args](
	                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
	                                                                  )
	                                                                  {
		                                                                  auto ViewConfig = SPtr->
			                                                                  GetViewConfig();
		                                                                  ViewConfig.CurtainWallTranlucent =
				                                                                  UKismetStringLibrary::Conv_StringToInt(
					                                                                   Args[0]
					                                                                  );
		                                                                  SPtr->UpdateViewConfig(ViewConfig, false);
	                                                                  },
	                                                                  true
	                                                                 );
}

void SmartCityCommand::SetShowFurniture(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	USceneInteractionWorldSystem::GetInstance()->SetInteractionOption(
	                                                                  USmartCitySuiteTags::Interaction_Interaction_WallTranlucent,
	                                                                  [&Args](
	                                                                  const TSharedPtr<FInteraction_Decorator>& SPtr
	                                                                  )
	                                                                  {
		                                                                  auto ViewConfig = SPtr->
			                                                                  GetViewConfig();
		                                                                  ViewConfig.FurnitureTranlucent =
				                                                                  UKismetStringLibrary::Conv_StringToInt(
					                                                                   Args[0]
					                                                                  );
		                                                                  SPtr->UpdateViewConfig(ViewConfig, false);
	                                                                  },
	                                                                  true
	                                                                 );
}

void SmartCityCommand::SetCameraPitch(
	const TArray<FString>& Args
	)
{
}

void SmartCityCommand::LocaterDeviceByID(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	UWebChannelWorldSystem::GetInstance()->OnInput(
												   FString::Printf(
																   TEXT(
																		R"({
    "CMD": "LocaterDeviceByID",
    "DeviceID": "%s"
})"
																	   ),
																   *Args[0]
																  )
												  );
}

void SmartCityCommand::LocaterSpaceByID(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	UWebChannelWorldSystem::GetInstance()->OnInput(
												   FString::Printf(
																   TEXT(
																		R"({
    "CMD": "LocaterSpaceByID",
    "Floor": "%s",
    "SpaceID": "%s"
})"
																	   ),
																   *Args[0],
																   *Args[1]
																  )
												  );
}

void SmartCityCommand::SetRelativeTransoform(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(6))
	{
		return;
	}

	UWebChannelWorldSystem::GetInstance()->OnInput(
												   FString::Printf(
																   TEXT(
																		R"({
    "CMD": "SetRelativeTransoform",
    "DeviceID": "%s",
    "Rotation_Pitch": "%s",
    "Rotation_Yaw": "%s",
    "Rotation_Roll": "%s",
    "Translation_X": "%s",
    "Translation_Y": "%s",
    "Translation_Z": "%s"
})"
																	   ),
																   *Args[0],
																   *Args[1],
																   *Args[2],
																   *Args[3],
																   *Args[4],
																   *Args[5],
																   *Args[6]
																  )
												  );
}

void SmartCityCommand::UpdateFloorDescription(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(1))
	{
		return;
	}

	UWebChannelWorldSystem::GetInstance()->OnInput(
												   FString::Printf(
																   TEXT(
																		R"({
    "CMD": "UpdateFloorDescription",
    "Floor": "%s",
    "FloorDescription": "%s"
})"
																	   ),
																   *Args[0],
																   *Args[1]
																  )
												  );
}

void SmartCityCommand::ViewSpeacialArea(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}

	if (UAssetRefMap::GetInstance()->ParamJson.Contains(Args[0]))
	{
		UWebChannelWorldSystem::GetInstance()->OnInput(UAssetRefMap::GetInstance()->ParamJson[Args[0]]
														  );
	}
}

void SmartCityCommand::UpdateSceneElementParam(
	const TArray<FString>& Args
	)
{
	if (!Args.IsValidIndex(2))
	{
		return;
	}

	
	
	UWebChannelWorldSystem::GetInstance()->OnInput(
												   FString::Printf(
																   TEXT(
																		R"({
    "CMD": "UpdateSceneElementParam",
    "SceneElements":{
	 "%s": {
            "%s": "%s"
        }
}
})"
																	   ),
																   *Args[0],
																   *Args[1],
																   *Args[2]
																  )
												  );
}

void SmartCityCommand::UpdateSceneElementParamByArea(
	const TArray<FString>& Args
	)
{
}
