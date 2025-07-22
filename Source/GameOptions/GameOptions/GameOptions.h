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

UCLASS(BlueprintType, Blueprintable)
class GAMEOPTIONS_API UGameOptions : public UGameUserSettings
{
	GENERATED_BODY()

public:
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

#pragma region 查看全局
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 RotYawSpeed = 80;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 RotPitchSpeed = 80;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 MoveSpeed = 1000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 MaxMoveSpeed = 12000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 Acceleration = 10000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 Deceleration = 20000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 CameraSpringArmSpeed = 500;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 DefautlCameraSpringArm = 8000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 MinCameraSpringArm = 1000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 MaxCameraSpringArm = 20000;
#pragma endregion

#pragma region 查看楼层
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	int32 LinetraceDistance = 10000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	FRotator ViewFloorRot = FRotator(-30, 30, 0);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	float ViewFloorFOV = 90.f;
#pragma endregion

#pragma region 查看单个设备
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	FRotator ViewDeviceRot = FRotator(-30, 30, 0);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	float ViewDeviceFOV = 90.f;
#pragma endregion

#pragma region 渲染
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Viewer")
	int32 FocusOutline = 1;
#pragma endregion

protected:
	
private:

};
