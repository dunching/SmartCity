// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "AssetRefBase.h"
#include "GenerateTypes.h"

#include "AssetRefMap.generated.h"

class AActor;
class UDataLayerAsset;
class ADatasmithSceneActor;
class URouteMarker;
class AFloorHelper;
class AElevator;
class APostProcessVolume;
class ASceneElementBase;

UCLASS(BlueprintType, Blueprintable)
class SMARTCITY_API UAssetRefMap : public UAssetRefBase
{
	GENERATED_BODY()

public:
	UAssetRefMap();

	static UAssetRefMap* GetInstance();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FSceneElementTypeHelper, TSubclassOf<ASceneElementBase>> NeedReplaceMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftObjectPtr<APostProcessVolume>> PostProcessVolumeMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, FSceneElementTypeHelper> SceneElementMap;

#pragma region 场景Actor
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FSceneElementConditional, FSceneElementMap> SceneActorMap;
#pragma endregion

#pragma region 建筑信息
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<int32, TSoftObjectPtr<AFloorHelper>> FloorHelpers;
#pragma endregion

#pragma region 电梯
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftObjectPtr<AElevator>> ElevatorMap;
#pragma endregion

#pragma region DataSmith
	/**
	 * 设备ID的前缀
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Datasmith_UniqueId;

	/**
	 * 设备名称的Key
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FString> NamePrifix;

	/**
	 * 设备类型的Key
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSet<FString> CatogoryPrifix;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString FJPG;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString XFJZ;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Space;
#pragma endregion

#pragma region 引用
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> SpaceMaterialInstance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<URouteMarker> RouteMarkerClass;
#pragma endregion
};
