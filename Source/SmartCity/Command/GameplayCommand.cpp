#include "GameplayCommand.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
#include "GameplayTagsLibrary.h"
#include "LogWriter.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "Tools.h"
#include "SceneInteractionWorldSystem.h"
#include "UI/HUD/MainHUD.h"
#include "UI/HUD/MainHUDLayout.h"

void SmartCityCommand::ReplyCameraTransform()
{
	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	PCPtr->GameplayTasksComponentPtr->StartGameplayTask<UGT_ReplyCameraTransform>([](UGT_ReplyCameraTransform* GTPtr)
	{
		if (GTPtr)
		{
			GTPtr->SeatTag = UGameplayTagsLibrary::Seat_Default;
		}
	});
}

void SmartCityCommand::SwitchViewArea(const TArray<FString>& Args)
{
	for (auto Iter : Args)
	{
		USceneInteractionWorldSystem::GetInstance()->SwitchViewArea(FGameplayTag::RequestGameplayTag(*Iter));
		return;
	}
}

void SmartCityCommand::SwitchMode(const TArray<FString>& Args)
{
}

void SmartCityCommand::TestAssetUserData()
{
	TArray<AActor*> ResultAry;
	UGameplayStatics::GetAllActorsOfClass(GetWorldImp(),
	                                      AStaticMeshActor::StaticClass(),
	                                      ResultAry);

	for (auto Iter : ResultAry)
	{
		auto STMPtr = Cast<AStaticMeshActor>(Iter);
		if (STMPtr)
		{
			auto AUDPtr = Cast<UDatasmithAssetUserData>(
				STMPtr->GetStaticMeshComponent()->GetAssetUserDataOfClass(
					UDatasmithAssetUserData::StaticClass()));
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

void SmartCityCommand::AddFeatureItem(const TArray< FString >& Args)
{
	if (!Args.IsValidIndex(0))
	{
		return;
	}
	
	auto HUDPtr = Cast<AMainHUD>(GEngine->GetFirstLocalPlayerController(GetWorldImp())->GetHUD());
	if (HUDPtr )
	{
		HUDPtr->GetMainHUDLayout()->InitalFeaturesItem(Args[0], Args);
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

	
}
