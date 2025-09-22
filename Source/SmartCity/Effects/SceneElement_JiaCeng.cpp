
#include "SceneElement_JiaCeng.h"

#include "ActorSequenceComponent.h"
#include "DatasmithAssetUserData.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "GameplayTagsLibrary.h"
#include "MessageBody.h"
#include "RouteMarker.h"
#include "SmartCitySuiteTags.h"
#include "WebChannelWorldSystem.h"

bool ISceneElement_JiaCengInterface::ProcessJiaCengLogic(
	const FSceneElementConditional& ConditionalSet
	) const
{
	auto TempEmptyContainer = FGameplayTagContainer::EmptyContainer;

	TempEmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor);

	if (ConditionalSet.ConditionalSet.HasAll(TempEmptyContainer))
	{
		TempEmptyContainer = FGameplayTagContainer::EmptyContainer;

		TempEmptyContainer.AddTag(USmartCitySuiteTags::Interaction_Area_Floor_F1J);

		// 是否是夹层
		if (ConditionalSet.ConditionalSet.HasAll(TempEmptyContainer))
		{
			if (bIsJiaCengg)
			{
			}
			else
			{
				return true;
			}
		}
	}

	return false;
}

void ISceneElement_JiaCengInterface::CheckIsJiaCeng(
	UDatasmithAssetUserData* AUDPtr
	)
{
	if (!AUDPtr)
	{
		return;
	}
	{
		{
			auto ID = AUDPtr->MetaData.Find(TEXT("Element*管线类型编号"));
			if (ID)
			{
				if (ID->Contains(TEXT("J")))
				{
					bIsJiaCengg = true;
					return;
				}
			}
		}
		{
			auto ID = AUDPtr->MetaData.Find(TEXT("Element*照明回路编号"));
			if (ID)
			{
				if (ID->Contains(TEXT("J")))
				{
					bIsJiaCengg = true;
					return;
				}
			}
		}
		{
			auto ID = AUDPtr->MetaData.Find(TEXT("Element*空调和新风回路编号"));
			if (ID)
			{
				if (ID->Contains(TEXT("J")))
				{
					bIsJiaCengg = true;
					return;
				}
			}
		}
	}
}
