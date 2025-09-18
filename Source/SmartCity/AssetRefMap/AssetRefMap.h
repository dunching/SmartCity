// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "AssetRefBase.h"
#include "GenerateTypes.h"
#include "BuildingGenerateTypes.h"

#include "AssetRefMap.generated.h"

class AActor;
class UDataLayerAsset;
class ADatasmithSceneActor;
class URouteMarker;
class AFloorHelper;
class ABuildingHelperBase;
class AElevator;
class APostProcessVolume;
class ASceneElementBase;
class APersonMark;
class AFireMark;
class ASceneElement_Space;

UCLASS(BlueprintType, Blueprintable)
class SMARTCITY_API UAssetRefMap : public UAssetRefBase
{
	GENERATED_BODY()

public:
	UAssetRefMap();

	static UAssetRefMap* GetInstance();

	/**
	 * 将指定的ADatasmithSceneActor引用的Actor替换成指定类型的ASceneElementBase
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<TSoftObjectPtr<ADatasmithSceneActor>, TSubclassOf<ASceneElementBase>> NeedReplaceByRef;

	/**
	 * 将指定的ADatasmithSceneActor的用户数据的引用的Actor替换成指定类型的ASceneElementBase
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FSceneElementReplaceHelper, TSubclassOf<ASceneElementBase>> NeedReplaceByUserData;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FSceneElementMergeHelper, TSubclassOf<ASceneElementBase>> NeedMergeByUserData;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FSceneSpace SpaceInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, FModeDecription> ModeDescription;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftObjectPtr<APostProcessVolume>> PostProcessVolumeMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 ViewLightingTime = 20;

#pragma region 场景Actor

#pragma endregion

#pragma region 建筑信息
	/**
	 * 层数
	 * 对应的Floor
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftObjectPtr<AFloorHelper>> FloorHelpers;
	
	/**
	 * 楼宇号
	 * 对应的楼宇
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftObjectPtr<ABuildingHelperBase>> BuildingHelpers;
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
#pragma endregion

#pragma region 引用
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName SpaceMaterialColorName = TEXT("BarrierColorPrimary");

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> SpaceMaterialInstance;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<ASceneElement_Space> SceneElement_SpaceClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<URouteMarker> SpaceRouteMarkerClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<URouteMarker> RouteMarkerClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<APersonMark> PersonMarkClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AFireMark> FireMarkClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> EnergyMaterialInst;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> WallTranslucentMatInst;
#pragma endregion
};
