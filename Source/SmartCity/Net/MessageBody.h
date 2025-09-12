// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"

#include "MessageBody.generated.h"

class FDecoratorBase;

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

USTRUCT()
struct FMessageBody_Send : public FMessageBody
{
	GENERATED_BODY()

public:
	FString GetJsonString() const;

	virtual TSharedPtr<FJsonObject> SerializeBody() const;
};

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

	int32 Pitch = 0;
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
struct FMessageBody_SelectedSpace : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_SelectedSpace();

	FString SpaceName;

	TArray<FString> DeviceIDAry;

protected:
	virtual TSharedPtr<FJsonObject> SerializeBody() const override;
};

USTRUCT()
struct FMessageBody_SelectedDevice : public FMessageBody_Send
{
	GENERATED_BODY()

	FMessageBody_SelectedDevice();

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
