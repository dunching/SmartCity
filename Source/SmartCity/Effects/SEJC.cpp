
#include "SEJC.h"

#include "DatasmithAssetUserData.h"

#include "AssetRefMap.h"
#include "GameplayTagsLibrary.h"
#include "SmartCitySuiteTags.h"

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
	bIsJiaCengg = false;
}
