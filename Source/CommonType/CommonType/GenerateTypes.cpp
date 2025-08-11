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