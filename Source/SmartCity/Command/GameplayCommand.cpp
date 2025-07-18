#include "GameplayCommand.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "DatasmithAssetUserData.h"
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
			auto AUDPtr = STMPtr->GetStaticMeshComponent()->GetStaticMesh()->GetAssetUserDataOfClass(
				UDatasmithAssetUserData::StaticClass());
			if (AUDPtr)
			{
			}
		}
	}
}
