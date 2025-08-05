#include "SceneElement_PWR_Pipe.h"

#include "Algorithm.h"
#include "Engine/OverlapResult.h"
#include "Engine/StaticMeshActor.h"

#include "AssetRefMap.h"
#include "CollisionDataStruct.h"
#include "DatasmithAssetUserData.h"
#include "FeatureWheel.h"
#include "GameplayTagsLibrary.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "SceneElement_DeviceBase.h"
#include "SceneInteractionWorldSystem.h"

ASceneElement_PWR_Pipe::ASceneElement_PWR_Pipe(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
}

void ASceneElement_PWR_Pipe::ReplaceImp(
	AActor* ActorPtr
	)
{
}

void ASceneElement_PWR_Pipe::SwitchInteractionType(
	EInteractionType InInteractionType
	)
{
}
