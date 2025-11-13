// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include "GameplayTagContainer.h"

#include "GameOptions.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct GAMEOPTIONS_API FGameplayFeatureKeyMap
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FKey Key;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString CMD;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Description;
};

USTRUCT(BlueprintType, Blueprintable)
struct GAMEOPTIONS_API FControlParam
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 RotYawSpeed = 8;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 RotPitchSpeed = 8;

	/**
	 * 移动输入的倍数
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 MoveSpeed = 10000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 MaxMoveSpeed = 10000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 Acceleration = 4*8000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 Deceleration = 8*8000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 CameraSpringArmSpeed = 1000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 DefautlCameraSpringArm = 8000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 MinCameraSpringArm = 1000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 MaxCameraSpringArm = 20000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	FRotator ViewRot = FRotator(-30, 10, 0);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	float CameraPitchMaxLimit = -10.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	float CameraPitchMinLimit = -70.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	float FOV = 90.f;
};

UCLASS(BlueprintType, Blueprintable)
class GAMEOPTIONS_API UGameOptions : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UGameOptions();
	
	UGameOptions(const FObjectInitializer& ObjectInitializer);
	
	static UGameOptions* GetInstance();

#pragma region 雷达控制

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Radar")
	float RadarQueryFrequency = 1.f / 24;

#pragma endregion

#pragma region 按键映射
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey ClickItem = EKeys::LeftMouseButton;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey RotBtn = EKeys::LeftMouseButton;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey MoveBtn = EKeys::RightMouseButton;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey MouseX = EKeys::MouseX;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey MouseY = EKeys::MouseY;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey MouseWheelAxis = EKeys::MouseWheelAxis;
#pragma endregion

#pragma region 查看周边
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FControlParam ViewPeripheryControlParam;
#pragma endregion

#pragma region 查看全局
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FControlParam ViewTowerControlParam;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FControlParam ViewSplitFloorControlParam;
#pragma endregion

#pragma region 查看楼层
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 LinetraceDistance = 10000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FControlParam ViewFloorControlParam;
#pragma endregion

#pragma region 查看能源
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FControlParam ViewFloorEnergyControlParam;
#pragma endregion

#pragma region 查看区域
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 ViewSpaceArmLen = 1350;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	FRotator ViewSpaceRot = FRotator(-60, 90, 0);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FControlParam ViewSpaceControlParam;
#pragma endregion

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FControlParam ViewSpecialAreaControlParam;
	
#pragma region 查看单个设备
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	FRotator ViewDeviceRot = FRotator(-30, 30, 0);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FControlParam ViewDeviceControlParam;
#pragma endregion

#pragma region 渲染
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	int32 FocusOutline = 1;
#pragma endregion

	bool bAllowRotByYaw = true;

	int32 MessageSplitNumber = 10000; 
	
protected:

private:
};

struct FViewConfig
{

	/**
	 * 墙体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 WallTranlucent = 100;

	/**
	 * 主体体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 PillarTranlucent = 100;

	/**
	 * 楼梯透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 StairsTranlucent = 100;

	/**
	 * 幕墙墙体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 CurtainWallTranlucent = 100;

	/**
	 * 家具 透明度
	 */
	int32 FurnitureTranlucent = 100;

};

enum class EInteractionType:uint8
{
	kDevice,
	kSpace,
};

struct FControlConfig
{
	bool bUseCustomPitchLimit = false;

	float ViewPitchMin = -45;

	float ViewPitchMax = -45;

	float ProcessPitchMin = -45;

	float ProcessPitchMax = -45;

	EInteractionType InteractionType = EInteractionType::kSpace;

	FGameplayTag CurrentWeather;

	int32 CurrentHour;
		
};
