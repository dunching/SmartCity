// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "PixelStreamingPlayerId.h"
#include "Engine/World.h"

#include "SceneInteractionDecorator.h"

#include "WebChannelWorldSystem.generated.h"

class FDecoratorBase;
struct FMessageBody;
struct FMessageBody_Send;
struct FMessageBody_Receive;

/*
 */
UCLASS()
class SMARTCITY_API UWebChannelWorldSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UWebChannelWorldSystem* GetInstance();

	void BindEvent();

	void InitializeDeserializeStrategies();

	void SendMessage(
		const TSharedPtr<FMessageBody_Send>& Message
		);

	UFUNCTION()
	void OnInput(
		const FString& Descriptor
		);

	/**
	 * Http 内部会做分批处理
	 * @param BimID 
	 * @param QueryComplete 
	 */
	void QueryDeviceID(
		const FString& BimID,
		const TFunction<void(
			bool bSuccess, const FString&
			)>& QueryComplete
		);

	FString QueryDeviceToken;

private:
	UFUNCTION()
	void OnConnectedToSignallingServerNative();

	void OnClosedConnectionNative(
		FString Str,
		FPixelStreamingPlayerId ID,
		bool bIsTrue
		);

	void NewConnectionNative(
		FString Str,
		FPixelStreamingPlayerId ID,
		bool bIsTrue
		);

	UFUNCTION()
	void OnAllConnectionsClosed(
		FString StreamerId
		);

	void MessageTickImp();

	TMap<FString, TSharedPtr<FMessageBody_Receive>> DeserializeStrategiesMap;

	FTimerHandle MessageTickTimerHandle;
};
