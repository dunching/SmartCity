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
#include "Kismet/KismetStringLibrary.h"

void SmartCityCommand::ReplyCameraTransform()
{
	USceneInteractionWorldSystem::GetInstance()->SwitchViewArea(UGameplayTagsLibrary::Interaction_Area_ExternalWall);
}

void SmartCityCommand::SwitchViewArea(
	const TArray<FString>& Args
	)
{
	for (auto Iter : Args)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchViewArea(FGameplayTag::RequestGameplayTag(*Iter));
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
