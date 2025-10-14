// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"

#include "MessageBody.generated.h"

class FDecoratorBase;
class ASceneElement_Space;
class ASceneElement_DeviceBase;

USTRUCT()
struct FMessageBody
{
	GENERATED_BODY()

	FMessageBody();

	virtual ~FMessageBody();

	FString GetCMDName() const;

public:
	static FString CMD;

	FString CMD_Name;

	FGuid Guid;
};

#pragma region Receive
USTRUCT()
struct FMessageBody_Receive : public FMessageBody
{
	GENERATED_BODY()

public:
	virtual void Deserialize(
		const FString& JsonStr
		);

	virtual void DoAction() const;
};

USTRUCT()
struct FMessageBody_Receive_AdjustCameraSeat : public FMessageBody_Receive
{
	GENERATED_BODY()

	FMessageBody_Receive_AdjustCameraSeat();

private:
	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	int32 MinPitch = 0;

	int32 MaxPitch = 0;

	bool bAllowRotByYaw = true;
};

USTRUCT()
struct FMessageBody_Receive_SwitchViewArea : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_SwitchViewArea();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FGameplayTag AreaTag;
};

USTRUCT()
struct FMessageBody_Receive_SwitchMode : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_SwitchMode();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FGameplayTag ModeTag;
};

USTRUCT()
struct FMessageBody_Receive_AdjustHour : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_AdjustHour();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	int32 Hour = 0;
};

USTRUCT()
struct FMessageBody_Receive_AdjustWeather : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_AdjustWeather();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FGameplayTag Weather;
};

USTRUCT()
struct FMessageBody_Receive_InteractionOption : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_InteractionOption();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	/**
	 * 墙体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 WallTranlucent = 100;

	/**
	 * 墙体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 PillarTranlucent = 100;

	/**
	 * 楼梯透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	int32 StairsTranlucent = 100;

	/**
	 * 幕墙墙体透明度 0 完全透明（隐藏） 100 完全不透明
	 */
	bool bShowCurtainWall = true;

	/**
	 * 是否显示家具
	 */
	bool bShowFurniture = true;

	bool bImmediatelyUpdate = true;
};

USTRUCT()
struct FMessageBody_Receive_LocaterDeviceByID : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_LocaterDeviceByID();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FString DeviceID;
};

USTRUCT()
struct FMessageBody_Receive_SwitchInteractionType : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_SwitchInteractionType();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FString InteractionType;

};
#pragma endregion

#pragma region Send
USTRUCT()
struct FMessageBody_Send : public FMessageBody
{
	GENERATED_BODY()

public:
	FString GetJsonString() const;

	virtual TSharedPtr<FJsonObject> SerializeBody() const;
};

USTRUCT()
struct FMessageBody_SelectedFloor : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_SelectedFloor();

	TMap<ASceneElement_Space*, TSet<ASceneElement_DeviceBase*>> SpacesMap;

protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

USTRUCT()
struct FMessageBody_SelectedSpace : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_SelectedSpace();

	FString SpaceName;

	struct FDeviceInfo
	{
		FString Type;

		FString DeviceID;

	};
	
	TArray<FDeviceInfo> DeviceIDAry;

protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

USTRUCT()
struct FMessageBody_SelectedDevice : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_SelectedDevice();

	TArray<FString> DeviceIDAry;

protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

USTRUCT()
struct FMessageBody_ViewDevice : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_ViewDevice();
  
	FString Type;

	FString DeviceID;

protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

USTRUCT()
struct FMessageBody_Test : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_Test();

	FString Text;

protected:
};

#pragma endregion
