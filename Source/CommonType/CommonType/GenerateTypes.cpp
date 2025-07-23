#include "GenerateTypes.h"

FSceneActorConditional::FSceneActorConditional()
{
}

FSceneActorConditional::FSceneActorConditional(
	const TSet<FGameplayTag>& InConditionalSet
	)
{
	for (const auto& Iter : InConditionalSet)
	{
		ConditionalSet.AddTag(Iter);
	}
}

uint32 GetTypeHash(
	const FSceneActorConditional& SceneActorConditional
	)
{
	uint32 HashCode = 0;
	for (const auto& Iter : SceneActorConditional.ConditionalSet)
	{
		HashCode = HashCombine(GetTypeHash(Iter), HashCode);
	}
	return HashCombine(HashCode, SceneActorConditional.bIsOnlyBulding);
}

TSceneActorConditionalKeyFuncs::KeyInitType TSceneActorConditionalKeyFuncs::GetSetKey(
	ElementInitType Element
	)
{
	return Element;
}

bool TSceneActorConditionalKeyFuncs::Matches(
	KeyInitType A,
	KeyInitType B
	)
{
	return (A.ConditionalSet == B.ConditionalSet) && (A.bIsOnlyBulding == B.bIsOnlyBulding);
}

uint32 TSceneActorConditionalKeyFuncs::GetKeyHash(
	KeyInitType Key
	)
{
	return GetTypeHash(Key);
}
