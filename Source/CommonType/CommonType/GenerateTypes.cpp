#include "GenerateTypes.h"

TFSceneElementTypeHelperKeyFuncs::KeyInitType TFSceneElementTypeHelperKeyFuncs::GetSetKey(
	ElementInitType Element
	)
{
	return Element;
}

bool TFSceneElementTypeHelperKeyFuncs::Matches(
	KeyInitType A,
	KeyInitType B
	)
{
	return (A.Key == B.Key) && (A.Value == B.Value);
}

uint32 TFSceneElementTypeHelperKeyFuncs::GetKeyHash(
	KeyInitType Key
	)
{
	return GetTypeHash(Key);
}

uint32 GetTypeHash(
	const FSceneElementTypeHelper& SceneElementTypeHelper
	)
{
	return HashCombine(GetTypeHash(SceneElementTypeHelper.Key), GetTypeHash(SceneElementTypeHelper.Value));
}

FSceneElementConditional::FSceneElementConditional()
{
}

FSceneElementConditional::FSceneElementConditional(
	const TSet<FGameplayTag>& InConditionalSet
	)
{
	for (const auto& Iter : InConditionalSet)
	{
		ConditionalSet.AddTag(Iter);
	}
}

uint32 GetTypeHash(
	const FSceneElementConditional& SceneActorConditional
	)
{
	uint32 HashCode = 0;
	for (const auto& Iter : SceneActorConditional.ConditionalSet)
	{
		HashCode = HashCombine(GetTypeHash(Iter), HashCode);
	}
	return HashCode;
}

TSceneElementConditionalKeyFuncs::KeyInitType TSceneElementConditionalKeyFuncs::GetSetKey(
	ElementInitType Element
	)
{
	return Element;
}

bool TSceneElementConditionalKeyFuncs::Matches(
	KeyInitType A,
	KeyInitType B
	)
{
	return (A.ConditionalSet == B.ConditionalSet);
}

uint32 TSceneElementConditionalKeyFuncs::GetKeyHash(
	KeyInitType Key
	)
{
	return GetTypeHash(Key);
}
