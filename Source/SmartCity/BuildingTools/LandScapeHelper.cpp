#include "LandScapeHelper.h"

#include "Landmark.h"
#include "SmartCitySuiteTags.h"

void ALandScapeHelper::SwitchInteractionType(
	const FSceneElementConditional& ConditionalSet
	)
{
	Super::SwitchInteractionType(ConditionalSet);

	{
		if (
			ConditionalSet.ConditionalSet.HasTagExact(USmartCitySuiteTags::Interaction_Area_Periphery)
		)
		{
			SetActorHiddenInGame(false);

			for (auto Iter : LandmarkAry)
			{
				auto MarkPtr = Iter.LoadSynchronous();
				if (MarkPtr)
				{
					MarkPtr->SetActorHiddenInGame(false);
				}
			}
			return;
		}
	}
	{
		SetActorHiddenInGame(true);

		for (auto Iter : LandmarkAry)
		{
			auto MarkPtr = Iter.LoadSynchronous();
			if (MarkPtr)
			{
				MarkPtr->SetActorHiddenInGame(true);
			}
		}
		return;
	}
}
