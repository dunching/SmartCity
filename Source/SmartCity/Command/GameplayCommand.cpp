#include "GameplayCommand.h"

#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"
#include "Tools.h"
#include "SceneInteractionWorldSystem.h"

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
		if (UAssetRefMap::GetInstance()->DataLayerAssetMap.Contains(FGameplayTag::RequestGameplayTag(*Iter)))
		{
			USceneInteractionWorldSystem::GetInstance()->SwitchViewArea(FGameplayTag::RequestGameplayTag(*Iter));
			return;
		}
	}
}

void SmartCityCommand::SwitchMode(const TArray<FString>& Args)
{
	
}
