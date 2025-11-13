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
class ATowerHelperBase;
class ALandScapeBase;
class AElevator;
class APostProcessVolume;
class ASceneElementBase;
class APersonMark;
class AFireMark;
class ASceneElement_Space;
class UFeatureWheel;
class ABuilding_Floor_Mask;
class AFloorHelper_Description;
class AViewerPawnBase;
class ASceneElement_RollerBlind;

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
	TMap<FGameplayTag, TSoftObjectPtr<ATowerHelperBase>> BuildingHelpers;

	/**
	 * 外景
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TSoftObjectPtr<ALandScapeBase>> LandScapeHelper;

	/**
	 * 需要单独处理的资源集
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, FSceneElementMap> SpecialDatasmith;

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
	TSoftObjectPtr<UMaterialInstance> EnergyPipeMaterialInst;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> EnergyDeviceMaterialInst;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> WallTranslucentMatInst;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> PillarTranslucentMatInst;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> StairsTranslucentMatInst;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> CurtainWallTranslucentMatInst;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<UMaterialInstance> FurnitureTranslucentMatInst;

	UPROPERTY(

		BlueprintReadWrite,
		EditAnywhere
	)
	TSubclassOf<UFeatureWheel> FeatureWheelClass;

	UPROPERTY(

		BlueprintReadWrite,
		EditAnywhere
	)
	TSubclassOf<ABuilding_Floor_Mask> Building_Floor_MaskClass;

	UPROPERTY(

		BlueprintReadWrite,
		EditAnywhere
	)
	TSubclassOf<AFloorHelper_Description> FloorHelper_DescriptionClass;

	UPROPERTY(

		BlueprintReadWrite,
		EditAnywhere
	)
	TSubclassOf<ASceneElement_RollerBlind> SceneElement_RollerBlindClass;

#pragma endregion
};
