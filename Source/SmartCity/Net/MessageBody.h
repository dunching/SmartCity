// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"
#include "GameOptions.h"

#include "MessageBody.generated.h"

class FDecoratorBase;
class ASceneElement_Space;
class ASceneElement_DeviceBase;
class AFloorHelper;
class AViewerPawnBase;

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

	void WriteLog()const;
protected:
	bool bIsWriteLog = true;
private:
	
	virtual void WriteLogDate(int32 &Day, int32& Hour)const;

	FString CurrentJsonStr;
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

	bool bUseClampPitch = true;

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
struct FMessageBody_Receive_UpdateViewConfig : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_UpdateViewConfig();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FViewConfig ViewConfig;

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
struct FMessageBody_Receive_LocaterSpaceByID : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_LocaterSpaceByID();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FGameplayTag Floor;

	FString SpaceID;
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

USTRUCT()
struct FMessageBody_Receive_UpdateRadarInfo : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_UpdateRadarInfo();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FString DeviceID;

	struct FUpdateRadarInfo
	{
		FVector2D Acceleration;

		FVector2D Position;

		FVector2D Velocity;

		FString TID;

		int32 EC = 0;

		int32 G = 0;

		int32 NumberOfTargets = 0;
	};

	TArray<FUpdateRadarInfo> Value;
};

USTRUCT()
struct FMessageBody_Receive_SetRelativeTransoform : public FMessageBody_Receive
{
	GENERATED_BODY()

public:
	FMessageBody_Receive_SetRelativeTransoform();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FString DeviceID;

	FTransform Transform;
};

USTRUCT()
struct FMessageBody_Receive_UpdateFloorDescription : public FMessageBody_Receive
{
	GENERATED_BODY()

	FMessageBody_Receive_UpdateFloorDescription();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FGameplayTag Floor;

	FString FloorDescription;
};

USTRUCT()
struct FMessageBody_Receive_ViewSpeacialArea : public FMessageBody_Receive
{
	GENERATED_BODY()

	FMessageBody_Receive_ViewSpeacialArea();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	TSet<FGameplayTag> FloorSet;

	TSet<FGameplayTag> PriorityHideFloorSet;

	FGameplayTag AreaTag;

	FString Seat;
};

USTRUCT()
struct FMessageBody_Receive_UpdateSceneElementParam : public FMessageBody_Receive
{
	GENERATED_BODY()

	FMessageBody_Receive_UpdateSceneElementParam();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	TMap<FString, TMap<FString, FString>> ExtensionParamMap;

	bool bImmediatelyUpdate = true;
};

USTRUCT()
struct FMessageBody_Receive_UpdateSceneElementParamByArea : public FMessageBody_Receive
{
	GENERATED_BODY()

	FMessageBody_Receive_UpdateSceneElementParamByArea();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	TMap<FString, TMap<FString, FString>> ExtensionParamMap;

	bool bImmediatelyUpdate = true;
};

USTRUCT()
struct FMessageBody_Receive_SelectedDevices : public FMessageBody_Receive
{
	GENERATED_BODY()

	FMessageBody_Receive_SelectedDevices();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	TArray<FString> DeviceIDAry;

};

USTRUCT()
struct FMessageBody_Receive_ClearSelectedDevices : public FMessageBody_Receive
{
	GENERATED_BODY()

	FMessageBody_Receive_ClearSelectedDevices();

	virtual void DoAction() const override;

};

USTRUCT()
struct FMessageBody_Receive_UpdateQueryDeviceToken : public FMessageBody_Receive
{
	GENERATED_BODY()

	FMessageBody_Receive_UpdateQueryDeviceToken();

	virtual void Deserialize(
		const FString& JsonStr
		) override;

	virtual void DoAction() const override;

	FString QueryDeviceToken;
	
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

	TArray<ASceneElement_PWR_Pipe*> PWR_PipeAry;

	TObjectPtr<AFloorHelper> FloorHelper = nullptr;

	TMap<FString, TSoftObjectPtr<AViewerPawnBase>> PresetBuildingCameraSeat;
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
struct FMessageBody_ClickedMonitor : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_ClickedMonitor();

	FString Type;

	FString DeviceID;

protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

USTRUCT()
struct FMessageBody_ViewConfigChanged : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_ViewConfigChanged();

	FViewConfig ViewConfig;

protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

USTRUCT()
struct FMessageBody_UE_Initialized : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_UE_Initialized();

protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

USTRUCT()
struct FMessageBody_UE_Tick : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_UE_Tick();

	FString Text;

	TSet<FString>ConnectedID;
	
protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

#pragma endregion
