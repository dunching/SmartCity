

#include "BuildingGenerateTypes.h"

FSceneElementConditional::FSceneElementConditional()
{
	
}

FSceneElementConditional FSceneElementConditional::EmptyConditional;

FSceneElementConditional::FSceneElementConditional(
	const TSet<FGameplayTag>& InConditionalSet
	)
{
	for (const auto& Iter : InConditionalSet)
	{
		ConditionalSet.AddTag(Iter);
	}
}
