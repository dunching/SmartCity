#include "GameplayCommand.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "ElevatorSubSystem.h"
#include "GameplayTagsLibrary.h"
#include "InputProcessorSubSystem_Imp.h"
#include "LogWriter.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "Tools.h"
#include "SceneInteractionWorldSystem.h"
#include "ViewBuildingProcessor.h"
#include "TourPawn.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "SceneElement_Space.h"
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

	for (auto Iter : Args)
	{
		UWebChannelWorldSystem::GetInstance()->OnInput(
		                                               FString::Printf(
		                                                               TEXT(
		                                                                    R"({
    "CMD": "AdjustCameraSeat",
    "Param": %s
})"
		                                                                   ),
		                                                               *Args[0]
		                                                              )
		                                              );
		return;
	}
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

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_CameraTransformLocaterByID>(
	false,
		 [&Args](
		 UGT_CameraTransformLocaterByID* GTPtr
		 )
		 {
			 if (GTPtr)
			 {
				 GTPtr->ID = FGuid(Args[0]);
			 }
		 }
		);
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
